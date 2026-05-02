/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * src/osal/osal_zephyr.c — Zephyr RTOS OSAL implementation.
 *
 */

#include <iol-master/osal.h>

#include <stdlib.h>
#include <stdbool.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(iol_osal, CONFIG_IOL_MASTER_LOG_LEVEL);

/* ================================================================== *
 *  Resource-pool sizing                                               *
 * ================================================================== */
#define TIMERS_PER_PORT   3
#define MUTEX_PER_PORT    6
#define EVT_PER_PORT      2
#define THREADS_PER_PORT  1   
#define MBOX_PER_PORT     2

/* Add a small overhead for the master-level thread and misc objects. */
#define TOTAL_TIMERS   (TIMERS_PER_PORT  * CONFIG_IOL_MASTER_NUM_CHANNELS + 2)
#define TOTAL_MUTEX    (MUTEX_PER_PORT   * CONFIG_IOL_MASTER_NUM_CHANNELS + 4)
#define TOTAL_EVT      (EVT_PER_PORT     * CONFIG_IOL_MASTER_NUM_CHANNELS + 4)
#define TOTAL_THREADS  (THREADS_PER_PORT * CONFIG_IOL_MASTER_NUM_CHANNELS + 1)
#define TOTAL_MBOX     (MBOX_PER_PORT    * CONFIG_IOL_MASTER_NUM_CHANNELS + 2)

/* ================================================================== *
 *  Static pools                                                       *
 * ================================================================== */
static uint8_t timer_cnt;
static uint8_t mutex_cnt;
static uint8_t evt_cnt;
static uint8_t thread_cnt;
static uint8_t mbox_cnt;

static os_timer_t     timer_pool [TOTAL_TIMERS];
static struct k_mutex mutex_pool [TOTAL_MUTEX];
static struct k_event evt_pool   [TOTAL_EVT];
static os_mbox_t      mbox_pool  [TOTAL_MBOX];
static struct k_thread thread_pool[TOTAL_THREADS];

/*
 * Thread stacks MUST be declared with Zephyr's K_THREAD_STACK_ARRAY_DEFINE.
 * The stkSto / stkSize arguments passed to os_thread_create() are ignored;
 * stacks are allocated from this pool instead.  Increase
 * CONFIG_IOL_MASTER_STACK_SIZE in prj.conf if tasks overflow.
 */
K_THREAD_STACK_ARRAY_DEFINE(thread_stacks, TOTAL_THREADS,
                             CONFIG_IOL_MASTER_STACK_SIZE);
static uint8_t stack_cnt;

/* ================================================================== *
 *  Internal helpers                                                   *
 * ================================================================== */
static k_timeout_t osal_to_zephyr_tmo(uint32_t osal_ms)
{
    return (osal_ms == OS_WAIT_FOREVER) ? K_FOREVER : K_MSEC(osal_ms);
}

/* ================================================================== *
 *  Memory                                                             *
 * ================================================================== */
void *os_malloc(size_t size)
{
    return k_malloc(size);
}

void os_free(void *ptr)
{
    k_free(ptr);
}

/* ================================================================== *
 *  Time                                                               *
 * ================================================================== */
void os_usleep(uint32_t us)
{
    k_sleep(K_USEC(us));
}

uint32_t os_get_current_time_us(void)
{
    /* k_uptime_get() returns ms; convert to µs (may overflow after ~71 min) */
    return (uint32_t)(k_uptime_get() * 1000U);
}

os_tick_t os_tick_current(void)
{
    return k_uptime_ticks();
}

os_tick_t os_tick_from_us(uint32_t us)
{
    return k_us_to_ticks_ceil32(us);
}

void os_tick_sleep(os_tick_t tick)
{
    k_sleep(K_TICKS(tick));
}

/* ================================================================== *
 *  Thread                                                             *
 * ================================================================== */
static void os_thread_entry_wrapper(void *p1, void *p2, void *p3)
{
    void (*fn)(void *) = (void (*)(void *))p1;
    fn(p2);
    ARG_UNUSED(p3);
}

/*
 * os_thread_create()
 *
 */
os_thread_t *os_thread_create(
    const char   *name,
    uint32_t      priority,
    void         *const stkSto,     /* ignored — see note above */
    const uint16_t stkSize,         /* ignored — see note above */
    void        (*entry)(void *arg),
    void         *arg)
{
    ARG_UNUSED(stkSto);
    ARG_UNUSED(stkSize);

    if (thread_cnt >= TOTAL_THREADS || stack_cnt >= TOTAL_THREADS) {
        LOG_ERR("os_thread_create: pool exhausted (%u/%u used)",
                thread_cnt, TOTAL_THREADS);
        return NULL;
    }

    struct k_thread    *t     = &thread_pool[thread_cnt];
    k_thread_stack_t   *stack = thread_stacks[stack_cnt];

    k_tid_t tid = k_thread_create(
        t, stack, CONFIG_IOL_MASTER_STACK_SIZE,
        os_thread_entry_wrapper,
        (void *)entry, arg, NULL,
        (int)priority,
        0,           /* options */
        K_NO_WAIT);

    if (tid == NULL) {
        LOG_ERR("os_thread_create: k_thread_create returned NULL");
        return NULL;
    }

    if (name != NULL) {
        k_thread_name_set(t, name);
    }

    thread_cnt++;
    stack_cnt++;

    LOG_DBG("Thread '%s' created (prio=%u)", name ? name : "?", priority);
    return t;
}

/* ================================================================== *
 *  Mutex                                                              *
 * ================================================================== */
os_mutex_t *os_mutex_create(void)
{
    if (mutex_cnt >= TOTAL_MUTEX) {
        LOG_ERR("os_mutex_create: pool exhausted");
        return NULL;
    }
    struct k_mutex *m = &mutex_pool[mutex_cnt++];
    k_mutex_init(m);
    return (os_mutex_t *)m;
}

void os_mutex_lock(os_mutex_t *mutex)
{
    k_mutex_lock((struct k_mutex *)mutex, K_FOREVER);
}

void os_mutex_unlock(os_mutex_t *mutex)
{
    k_mutex_unlock((struct k_mutex *)mutex);
}

void os_mutex_destroy(os_mutex_t *mutex)
{
    ARG_UNUSED(mutex);
    if (mutex_cnt > 0) {
        mutex_cnt--;
    }
}

/* ================================================================== *
 *  Semaphore                                                          *
 *  Allocated from heap (not pool) because they are used sparsely.    *
 * ================================================================== */
os_sem_t *os_sem_create(size_t count)
{
    struct k_sem *s = k_malloc(sizeof(struct k_sem));
    if (s == NULL) {
        LOG_ERR("os_sem_create: k_malloc failed");
        return NULL;
    }
    k_sem_init(s, (unsigned int)count, K_SEM_MAX_LIMIT);
    return (os_sem_t *)s;
}

bool os_sem_wait(os_sem_t *sem, uint32_t time)
{
    int rc = k_sem_take((struct k_sem *)sem, osal_to_zephyr_tmo(time));
    return (rc != 0); /* true = timed out */
}

void os_sem_signal(os_sem_t *sem)
{
    k_sem_give((struct k_sem *)sem);
}

void os_sem_destroy(os_sem_t *sem)
{
    k_free(sem);
}

/* ================================================================== *
 *  Event flags — k_event provides a direct 32-bit flag word          *
 * ================================================================== */
os_event_t *os_event_create(void)
{
    if (evt_cnt >= TOTAL_EVT) {
        LOG_ERR("os_event_create: pool exhausted");
        return NULL;
    }
    struct k_event *e = &evt_pool[evt_cnt++];
    k_event_init(e);
    return (os_event_t *)e;
}

/*
 * os_event_wait()
 *
 */
bool os_event_wait(os_event_t *event, uint32_t mask,
                   uint32_t *value, uint32_t time)
{
    uint32_t bits = k_event_wait((struct k_event *)event, mask,
                                 false,  /* do not clear on match */
                                 osal_to_zephyr_tmo(time));
    *value = bits & mask;
    return (*value == 0); /* true → timed out */
}

void os_event_set(os_event_t *event, uint32_t value)
{
    k_event_post((struct k_event *)event, value);
}

void os_event_clr(os_event_t *event, uint32_t value)
{
    k_event_clear((struct k_event *)event, value);
}

void os_event_destroy(os_event_t *event)
{
    ARG_UNUSED(event);
    if (evt_cnt > 0) {
        evt_cnt--;
    }
}

/* ================================================================== *
 *  Mailbox — pointer ring-buffer                                      *
 *  Signal bits:  0x01 = data available,  0x02 = space available      *
 * ================================================================== */
os_mbox_t *os_mbox_create(size_t size)
{
    if (mbox_cnt >= TOTAL_MBOX) {
        LOG_ERR("os_mbox_create: pool exhausted");
        return NULL;
    }
    if (size > MAX_MAILBOX_MSGS) {
        LOG_ERR("os_mbox_create: requested %zu > MAX_MAILBOX_MSGS %d",
                size, MAX_MAILBOX_MSGS);
        return NULL;
    }
    os_mbox_t *mb = &mbox_pool[mbox_cnt++];
    k_mutex_init(&mb->mutex);
    k_event_init(&mb->evt);
    mb->r     = 0;
    mb->w     = 0;
    mb->count = 0;
    mb->size  = (uint32_t)size;
    return mb;
}

bool os_mbox_fetch(os_mbox_t *mbox, void **msg, uint32_t time)
{
    k_timeout_t tmo = osal_to_zephyr_tmo(time);

    /* Fast path — data already available */
    k_mutex_lock(&mbox->mutex, K_FOREVER);
    if (mbox->count == 0) {
        k_mutex_unlock(&mbox->mutex);
        /* Block until data arrives or timeout */
        uint32_t bits = k_event_wait(&mbox->evt, 0x01U, true, tmo);
        if (bits == 0) {
            return true; /* timeout */
        }
        k_mutex_lock(&mbox->mutex, K_FOREVER);
    }

    *msg       = mbox->msg[mbox->r];
    mbox->r    = (mbox->r + 1) % mbox->size;
    mbox->count--;
    k_event_post(&mbox->evt, 0x02U); /* signal space available */
    k_mutex_unlock(&mbox->mutex);
    return false;
}

bool os_mbox_post(os_mbox_t *mbox, void *msg, uint32_t time)
{
    k_timeout_t tmo = osal_to_zephyr_tmo(time);

    /* Fast path — space already available */
    k_mutex_lock(&mbox->mutex, K_FOREVER);
    if (mbox->count == mbox->size) {
        k_mutex_unlock(&mbox->mutex);
        /* Block until space opens or timeout */
        uint32_t bits = k_event_wait(&mbox->evt, 0x02U, true, tmo);
        if (bits == 0) {
            return true; /* timeout */
        }
        k_mutex_lock(&mbox->mutex, K_FOREVER);
    }

    mbox->msg[mbox->w] = msg;
    mbox->w    = (mbox->w + 1) % mbox->size;
    mbox->count++;
    k_event_post(&mbox->evt, 0x01U); /* signal data available */
    k_mutex_unlock(&mbox->mutex);
    return false;
}

void os_mbox_destroy(os_mbox_t *mbox)
{
    ARG_UNUSED(mbox);
    if (mbox_cnt > 0) {
        mbox_cnt--;
    }
}

/* ================================================================== *
 *  Timer                                                              *
 * ================================================================== */
static void timer_zephyr_cb(struct k_timer *ztimer)
{
    os_timer_t *t = (os_timer_t *)ztimer->user_data;
    if (t->fn != NULL) {
        t->fn(t, t->arg);
    }
}

os_timer_t *os_timer_create(
    uint32_t us,
    void (*fn)(os_timer_t *, void *arg),
    void *arg,
    bool oneshot)
{
    if (timer_cnt >= TOTAL_TIMERS) {
        LOG_ERR("os_timer_create: pool exhausted");
        return NULL;
    }
    os_timer_t *t = &timer_pool[timer_cnt++];
    t->fn         = fn;
    t->arg        = arg;
    t->us         = us;
    t->one_shot   = oneshot;

    k_timer_init(&t->handle, timer_zephyr_cb, NULL);
    t->handle.user_data = t;
    return t;
}

void os_timer_set(os_timer_t *timer, uint32_t us)
{
    timer->us = us;
}

void os_timer_start(os_timer_t *timer)
{
    k_timeout_t dur    = K_USEC(timer->us);
    /* For one-shot: period = K_NO_WAIT stops the timer after first expiry. */
    k_timeout_t period = timer->one_shot ? K_NO_WAIT : K_USEC(timer->us);
    k_timer_start(&timer->handle, dur, period);
}

void os_timer_stop(os_timer_t *timer)
{
    k_timer_stop(&timer->handle);
}

void os_timer_destroy(os_timer_t *timer)
{
    k_timer_stop(&timer->handle);
    ARG_UNUSED(timer);
    if (timer_cnt > 0) {
        timer_cnt--;
    }
}