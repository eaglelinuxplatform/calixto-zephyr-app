#define DT_DRV_COMPAT calixto_ism

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/__assert.h>

#include "drivers/ism.h"

LOG_MODULE_REGISTER(ism_driver, CONFIG_ISM_LOG_LEVEL);

#define ISM_SAMPLE_PERIOD_MS    CONFIG_ISM_SAMPLE_PERIOD_MS
#define ISM_CONSECUTIVE_SAMPLES CONFIG_ISM_CONSECUTIVE_SAMPLES
#define ISM_THREAD_STACK_SIZE   CONFIG_ISM_THREAD_STACK_SIZE
#define ISM_THREAD_PRIORITY     CONFIG_ISM_THREAD_PRIORITY

/* Maximum channels supported: one per bit in a uint16_t */
#define ISM_MAX_CHANNELS        16U

/*
 * Per-channel consecutive-sample counter.
 *
 * per channel:
 *   +ISM_CONSECUTIVE_SAMPLES  -> channel is stably HIGH
 *   -ISM_CONSECUTIVE_SAMPLES  -> channel is stably LOW
 */
struct ism_data {
    struct k_mutex    lock;
    struct k_thread   thread;
    k_thread_stack_t *stack;
    ism_callback_t    callback;
    uint16_t          confirmed_state;
    uint16_t          valid_mask;

    int8_t            ctr[ISM_MAX_CHANNELS];

    volatile bool     abort;
    bool              running;
};

struct ism_channel_cfg {
    struct gpio_dt_spec gpio;
};

struct ism_config {
    const struct ism_channel_cfg *channels;
    uint8_t                       num_channels;
};


/**
 * @brief Update the saturating counter for one channel and return the new
 *        stable state if the counter has reached its limit, or -1 if the
 *        channel is still bouncing.
 *
 * @param ctr   Pointer to the channel's int8_t counter.
 * @param raw   Current GPIO reading (0 or 1).
 * @return  0  – stably LOW
 *          1  – stably HIGH
 *         -1  – not yet stable
 */
static int debounce_update(int8_t *ctr, uint8_t raw)
{
    const int8_t limit = (int8_t)ISM_CONSECUTIVE_SAMPLES;

    if (raw) {
        if (*ctr < limit) {
            (*ctr)++;
        }
    } else {
        if (*ctr > -limit) {
            (*ctr)--;
        }
    }

    if (*ctr >= limit) {
        return 1;
    }

    if (*ctr <= -limit) {
        return 0;
    }

    return -1;
}

static void ism_poll_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    const struct device     *dev  = (const struct device *)p1;
    const struct ism_config *cfg  = dev->config;
    struct ism_data         *data = dev->data;

    for (uint8_t ch = 0; ch < cfg->num_channels; ch++) {
        int raw = gpio_pin_get_dt(&cfg->channels[ch].gpio);

        if (raw < 0) {
            LOG_WRN("ch%u: initial read failed (err %d); assuming LOW", ch, raw);
            raw = 0;
        }

        data->ctr[ch] = raw ? (int8_t)ISM_CONSECUTIVE_SAMPLES
                             : -(int8_t)ISM_CONSECUTIVE_SAMPLES;

        if (raw) {
            data->confirmed_state |=  (uint16_t)(1U << ch);
        } else {
            data->confirmed_state &= ~(uint16_t)(1U << ch);
        }

        data->valid_mask |= (uint16_t)(1U << ch);

        LOG_DBG("ch%u: initial state = %u", ch, (unsigned)raw);
    }

    while (!data->abort) {

        k_msleep(ISM_SAMPLE_PERIOD_MS);

        if (data->abort) {
            break;
        }

        k_mutex_lock(&data->lock, K_FOREVER);

        for (uint8_t ch = 0; ch < cfg->num_channels; ch++) {

            int raw = gpio_pin_get_dt(&cfg->channels[ch].gpio);

            if (raw < 0) {
                LOG_ERR("ch%u: GPIO read error (err %d); skipping sample",
                        ch, raw);
                continue;
            }

            int stable = debounce_update(&data->ctr[ch], (uint8_t)raw);

            if (stable < 0) {
                /* Channel still bouncing – nothing to report */
                LOG_DBG("ch%u: still bouncing", ch);
                continue;
            }

            uint8_t old_bit = (uint8_t)((data->confirmed_state >> ch) & 1U);

            if ((uint8_t)stable == old_bit) {
                continue;
            }

            if (stable) {
                data->confirmed_state |=  (uint16_t)(1U << ch);
            } else {
                data->confirmed_state &= ~(uint16_t)(1U << ch);
            }

            data->valid_mask |= (uint16_t)(1U << ch);

            ism_callback_t cb = data->callback;

            k_mutex_unlock(&data->lock);

            if (cb != NULL) {
                cb(ch, (uint8_t)stable);
            }

            k_mutex_lock(&data->lock, K_FOREVER);
        }

        k_mutex_unlock(&data->lock);
    }

    LOG_INF("ISM poll thread exiting");
}

static int _ism_init(const struct device *dev,
                     ism_callback_t       cb)
{
    const struct ism_config *cfg  = dev->config;
    struct ism_data         *data = dev->data;

    if (cb == NULL) {
        LOG_ERR("ISM_init: callback must not be NULL");
        return -EINVAL;
    }

    if (cfg->num_channels == 0U) {
        LOG_ERR("ISM_init: no channels in device-tree");
        return -ENODEV;
    }

    if (cfg->num_channels > ISM_MAX_CHANNELS) {
        LOG_ERR("ISM_init: %u channels requested, max is %u",
                cfg->num_channels, ISM_MAX_CHANNELS);
        return -EINVAL;
    }

    if (data->running) {
        LOG_WRN("ISM_init: already running - stopping first");
        data->abort = true;
        k_thread_join(&data->thread,
                      K_MSEC(2U * ISM_SAMPLE_PERIOD_MS + 50U));
        data->running = false;
        data->abort   = false;
    }

    k_mutex_lock(&data->lock, K_FOREVER);

    data->callback        = cb;
    data->abort           = false;
    data->confirmed_state = 0U;
    data->valid_mask      = 0U;

    (void)memset(data->ctr, 0, sizeof(data->ctr));

    for (uint8_t ch = 0; ch < cfg->num_channels; ch++) {
        const struct gpio_dt_spec *g = &cfg->channels[ch].gpio;

        if (!gpio_is_ready_dt(g)) {
            LOG_ERR("ch%u: GPIO controller not ready", ch);
            k_mutex_unlock(&data->lock);
            return -ENODEV;
        }

        int err = gpio_pin_configure_dt(g, GPIO_INPUT);

        if (err != 0) {
            LOG_ERR("ch%u: gpio_pin_configure_dt failed (err %d)", ch, err);
            k_mutex_unlock(&data->lock);
            return err;
        }

        LOG_DBG("ch%u: GPIO configured - port=%s pin=%u",
                ch, g->port->name, g->pin);
    }

    k_mutex_unlock(&data->lock);

    k_tid_t tid = k_thread_create(
        &data->thread,
        data->stack,
        ISM_THREAD_STACK_SIZE,
        ism_poll_thread,
        (void *)dev, NULL, NULL,
        ISM_THREAD_PRIORITY,
        0,
        K_NO_WAIT);

    if (tid == NULL) {
        LOG_ERR("ISM_init: k_thread_create failed");
        return -ENOMEM;
    }

    k_thread_name_set(&data->thread, "ism_poll");
    data->running = true;

    LOG_INF("ISM ready - %u ch, %u ms period, %u consec. samples required",
            cfg->num_channels, ISM_SAMPLE_PERIOD_MS, ISM_CONSECUTIVE_SAMPLES);

    return 0;
}

static int _ism_deinit(const struct device *dev)
{
    struct ism_data *data = dev->data;

    if (!data->running) {
        LOG_WRN("ISM_deinit: not running");
        return 0;
    }

    data->abort = true;

    int err = k_thread_join(&data->thread,
                            K_MSEC(2U * ISM_SAMPLE_PERIOD_MS + 50U));
    if (err != 0) {
        LOG_WRN("ISM_deinit: thread join timed out (err %d)", err);
    }

    k_mutex_lock(&data->lock, K_FOREVER);
    data->callback = NULL;
    data->running  = false;
    k_mutex_unlock(&data->lock);

    LOG_INF("ISM deinitialized");
    return 0;
}

static int _ism_rd_chnl_state(const struct device *dev,
                               uint8_t              chnl,
                               uint16_t            *chnl_state)
{
    const struct ism_config *cfg  = dev->config;
    struct ism_data         *data = dev->data;

    if (chnl_state == NULL) {
        return -EINVAL;
    }

    if (chnl >= cfg->num_channels) {
        LOG_ERR("rd_chnl_state: ch%u out of range (max %u)",
                chnl, cfg->num_channels - 1U);
        return -ENODEV;
    }

    k_mutex_lock(&data->lock, K_FOREVER);

    if (!(data->valid_mask & (uint16_t)(1U << chnl))) {
        k_mutex_unlock(&data->lock);
        return -EAGAIN;
    }

    *chnl_state = (uint16_t)((data->confirmed_state >> chnl) & 1U);

    k_mutex_unlock(&data->lock);

    return 0;
}


static const struct ism_driver_api ism_api = {
    .init          = _ism_init,
    .deinit        = _ism_deinit,
    .rd_chnl_state = _ism_rd_chnl_state,
};


static int ism_device_init_fn(const struct device *dev)
{
    const struct ism_config *cfg = dev->config;

    if (cfg->num_channels > ISM_MAX_CHANNELS) {
        LOG_ERR("ISM: %u channels exceeds hard limit of %u",
                cfg->num_channels, ISM_MAX_CHANNELS);
        return -EINVAL;
    }

    for (uint8_t ch = 0; ch < cfg->num_channels; ch++) {
        if (!gpio_is_ready_dt(&cfg->channels[ch].gpio)) {
            LOG_ERR("ch%u: GPIO controller not ready at boot", ch);
            return -ENODEV;
        }
    }

    LOG_DBG("ISM early-init OK (%u channels)", cfg->num_channels);
    return 0;
}


#define ISM_CHANNEL_INIT(node_id) \
    { .gpio = GPIO_DT_SPEC_GET(node_id, gpios) },

#define ISM_DEVICE_INIT(inst)                                                   \
    K_THREAD_STACK_DEFINE(ism_thread_stack_##inst,                              \
                          CONFIG_ISM_THREAD_STACK_SIZE);                        \
                                                                                \
    static const struct ism_channel_cfg ism_channels_##inst[] = {              \
        DT_INST_FOREACH_CHILD(inst, ISM_CHANNEL_INIT)                           \
    };                                                                          \
    static const struct ism_config ism_cfg_##inst = {                           \
        .channels     = ism_channels_##inst,                                    \
        .num_channels = ARRAY_SIZE(ism_channels_##inst),                        \
    };                                                                          \
    static struct ism_data ism_data_##inst = {                                  \
        .lock  = Z_MUTEX_INITIALIZER(ism_data_##inst.lock),                     \
        .stack = ism_thread_stack_##inst,                                       \
    };                                                                          \
    DEVICE_DT_INST_DEFINE(inst,                                                 \
                          ism_device_init_fn,                                   \
                          NULL,                                                 \
                          &ism_data_##inst,                                     \
                          &ism_cfg_##inst,                                      \
                          POST_KERNEL,                                          \
                          CONFIG_ISM_INIT_PRIORITY,                             \
                          &ism_api);

DT_INST_FOREACH_STATUS_OKAY(ISM_DEVICE_INIT)