/**
  ******************************************************************************
  * @file    iol_main.c
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    02-Feb-2026
  * @brief   This file provides functions to manage the following
  *          breif the functionalities here:
  *           - 1.
  *
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "iol-master/iol_main.h"
#include "iol-master/iol_cce4511.h"
#include <iol-master/iol_cce4511_pl.h>


#include "iol-master/iol_pl.h"
#include "iol-master/iol_dl.h"
#include "iol-master/iol_sm.h"
#include "iol-master/iol_mw.h"

#include <stdlib.h> /* calloc */
#include <string.h>

#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "iol-master/osal_spi.h"
#include "iol-master/cce4511_conf.h"


/* ---------------------------------------------------------------------------
 * CCE4511 hardware resources 
 * ---------------------------------------------------------------------------*/
#define CCE4511_NODE  DT_NODELABEL(cce4511_0)

static const struct spi_dt_spec cce4511_spi =
    SPI_DT_SPEC_GET(CCE4511_NODE, CCE4511_SPI_OPERATION, 0);

static const struct gpio_dt_spec cce4511_irq_gpio =
    GPIO_DT_SPEC_GET(CCE4511_NODE, irq_gpios);

static struct gpio_callback cce4511_gpio_cb;

static void gpio_isr_handler (
   const struct device * port,
   struct gpio_callback * cb,
   gpio_port_pins_t       pins)
{
   ARG_UNUSED (port);
   ARG_UNUSED (cb);
   ARG_UNUSED (pins);

   if (g_iolink_drv != NULL)
   {
      iolink_cce4511_isr ((void *)g_iolink_drv);
   }
}

static int iol_hw_init (void)
{
   if (!spi_is_ready_dt (&cce4511_spi))
   {
      IOL_LOG_ERR ("CCE4511 SPI bus not ready - check DTS overlay\n");
      return -ENODEV;
   }

   iol_spi_set_device (&cce4511_spi);

   if (!gpio_is_ready_dt (&cce4511_irq_gpio))
   {
      IOL_LOG_ERR ("CCE4511 IRQ GPIO not ready - check DTS irq-gpios\n");
      return -ENODEV;
   }

   int rc = gpio_pin_configure_dt (&cce4511_irq_gpio, GPIO_INPUT);
   if (rc != 0)
   {
      IOL_LOG_ERR ("gpio_pin_configure_dt(IRQ) failed: %d\n", rc);
      return rc;
   }

   rc = gpio_pin_interrupt_configure_dt (&cce4511_irq_gpio,
                                          GPIO_INT_EDGE_FALLING);
   if (rc != 0)
   {
      IOL_LOG_ERR ("gpio_pin_interrupt_configure_dt failed: %d\n", rc);
      return rc;
   }

   gpio_init_callback (&cce4511_gpio_cb,
                       gpio_isr_handler,
                       BIT (cce4511_irq_gpio.pin));

   rc = gpio_add_callback (cce4511_irq_gpio.port, &cce4511_gpio_cb);
   if (rc != 0)
   {
      IOL_LOG_ERR ("gpio_add_callback failed: %d\n", rc);
      return rc;
   }

   IOL_LOG_DBG ("CCE4511 HW init done - IRQ GPIO pin %d\n",
                cce4511_irq_gpio.pin);
   return 0;
}

enum { MASTER_RW_DONE_BIT = 0x1 };
enum { MASTER_RW_TIMEOUT_MS = 5000 };

typedef struct
{
   bool in_use;
   os_event_t * ev;
   iolink_error_t res;
   uint8_t * out_buf;
   uint8_t * out_len;
} master_read_ctx_t;

typedef struct
{
   bool in_use;
   os_event_t * ev;
   iolink_error_t res;
} master_write_ctx_t;

static master_read_ctx_t s_master_read_ctx[IOLINK_NUM_PORTS];
static master_write_ctx_t s_master_write_ctx[IOLINK_NUM_PORTS];

static void master_read_cnf_cb (
   iolink_port_t * cb_port,
   uint8_t cb_len,
   const uint8_t * cb_data,
   iolink_error_t errortype)
{
   uint8_t pnum = iolink_get_portnumber (cb_port);
   if ((pnum == 0) || (pnum > IOLINK_NUM_PORTS))
   {
      return;
   }

   master_read_ctx_t * ctx = &s_master_read_ctx[pnum - 1];
   ctx->res = errortype;

   if ((errortype == IOLINK_ERROR_NONE) && (cb_data != NULL) &&
       (ctx->out_buf != NULL) && (ctx->out_len != NULL))
   {
      memcpy (ctx->out_buf, cb_data, cb_len);
      *(ctx->out_len) = cb_len;
   }
   else if (ctx->out_len != NULL)
   {
      *(ctx->out_len) = 0;
   }

   if (ctx->ev)
   {
      os_event_set (ctx->ev, MASTER_RW_DONE_BIT);
   }
}

static void master_write_cnf_cb (
   iolink_port_t * cb_port,
   iolink_error_t errortype)
{
   uint8_t pnum = iolink_get_portnumber (cb_port);
   if ((pnum == 0) || (pnum > IOLINK_NUM_PORTS))
   {
      return;
   }

   master_write_ctx_t * ctx = &s_master_write_ctx[pnum - 1];
   ctx->res = errortype;

   if (ctx->ev)
   {
      os_event_set (ctx->ev, MASTER_RW_DONE_BIT);
   }
}

/**
 * @file
 * @brief Handler
 *
 */

#define IOLINK_MASTER_JOB_CNT     100
#define IOLINK_MASTER_JOB_API_CNT 50

typedef struct iolink_port
{
   iolink_m_t * master;
   uint8_t portnumber;
   iolink_pl_port_t pl;
   iolink_dl_t dl;
   iolink_sm_port_t sm;
   iolink_mw_port_t mw;
   iolink_port_info_t port_info;
} iolink_port_t;


typedef struct iolink_m
{
   bool has_exited;
   os_thread_t  *thread;
   os_mbox_t * mbox;           /* Mailbox for job submission */
   os_mbox_t  *mbox_avail;     /* Mailbox for available jobs */
   os_mbox_t  *mbox_api_avail; /* Mailbox for available API (external) jobs */
   iolink_job_t job[IOLINK_MASTER_JOB_CNT];
   iolink_job_t job_api[IOLINK_MASTER_JOB_API_CNT];

   void * cb_arg; /* Callback opaque argument */

   /* data callback */
   void (*user_cb) (
      uint8_t portnumber,
      void * arg,
	  IOL_data_ind data_type,
      uint8_t data_len,
      const uint8_t * data);

   uint8_t port_cnt;
   struct iolink_port ports[];
} iolink_m_t;

iolink_m_t * the_master = NULL;

static iolink_transmission_rate_t mhmode_to_transmission_rate (
   iolink_mhmode_t mhmode)
{
   iolink_transmission_rate_t res = IOLINK_TRANSMISSION_RATE_NOT_DETECTED;

   switch (mhmode)
   {
   case IOLINK_MHMODE_COM1:
      res = IOLINK_TRANSMISSION_RATE_COM1;
      break;
   case IOLINK_MHMODE_COM2:
      res = IOLINK_TRANSMISSION_RATE_COM2;
      break;
   case IOLINK_MHMODE_COM3:
      res = IOLINK_TRANSMISSION_RATE_COM3;
      break;
   default:
      break;
   }

   return res;
}

// static iolink_error_t portnumber_to_iolinkport (
//    uint8_t portnumber,
//    iolink_port_t ** port)
// {
//    uint8_t port_index = portnumber - 1;

//    *port = NULL;

//    if (the_master == NULL)
//    {
//       return IOLINK_ERROR_STATE_INVALID;
//    }

//    if ((portnumber == 0) || (port_index >= the_master->port_cnt))
//    {
//       return IOLINK_ERROR_PARAMETER_CONFLICT;
//    }

//    *port = &the_master->ports[port_index];

//    return IOLINK_ERROR_NONE;
// }

static void iolink_main (void * arg)
{
   iolink_m_t * master = arg;
   bool running = true;

   while (running)
   {
      iolink_job_t * job;

      CC_ASSERT (master->mbox_avail != NULL);
      os_mbox_fetch (master->mbox, (void **)&job, OS_WAIT_FOREVER);

      CC_ASSERT (job != NULL);

      switch (job->type)
      {
      case IOLINK_JOB_PD_EVENT:
         if (master->user_cb)
         {
            master->user_cb (
               iolink_get_portnumber (job->port),
               master->cb_arg,
			   PD_DATA_IND,
               job->pd_event.data_len,
               job->pd_event.data);
         }
         job->type     = IOLINK_JOB_NONE;
         job->callback = NULL;
         os_mbox_post (master->mbox_avail, job, 0);
         break;
      case IOLINK_JOB_OPERATE_EVENT:
          if (master->user_cb)
          {
              master->user_cb(
                  iolink_get_portnumber(job->port),
                  master->cb_arg,
                  PORT_OPERATE_IND,
                  0,
                  NULL);
          }
          job->type     = IOLINK_JOB_NONE;
          job->callback = NULL;
          os_mbox_post(master->mbox_avail, job, 0);
          break;
      case IOLINK_JOB_COMLOST_EVENT:
          if (master->user_cb)
          {
              master->user_cb(iolink_get_portnumber(job->port),
                              master->cb_arg, PORT_COMLOST_IND, 0, NULL);
          }
          job->type     = IOLINK_JOB_NONE;
          job->callback = NULL;
          os_mbox_post(master->mbox_avail, job, 0);
          break;
      case IOLINK_JOB_SM_OPERATE_REQ:
      case IOLINK_JOB_SM_SET_PORT_CFG_REQ:
      case IOLINK_JOB_DL_MODE_IND:
      case IOLINK_JOB_DL_READ_CNF:
      case IOLINK_JOB_DL_WRITE_CNF:
      case IOLINK_JOB_DL_WRITE_DEVMODE_CNF:
      case IOLINK_JOB_DL_READPARAM_CNF:
      case IOLINK_JOB_DL_WRITEPARAM_CNF:
      case IOLINK_JOB_DL_ISDU_TRANS_CNF:
      case IOLINK_JOB_DL_PDINPUT_TRANS_IND:
      case IOLINK_JOB_MW_CONTROL_CNF:
      case IOLINK_JOB_MW_READ_REQ:
      case IOLINK_JOB_MW_READ_CNF:
      case IOLINK_JOB_MW_WRITE_REQ:
      case IOLINK_JOB_MW_WRITE_CNF:

      case IOLINK_JOB_SM_PORT_MODE_IND:
      case IOLINK_JOB_DL_EVENT_IND:
      case IOLINK_JOB_DL_CONTROL_IND:
      case IOLINK_JOB_DS_STARTUP:
      case IOLINK_JOB_DS_DELETE:
      case IOLINK_JOB_DS_INIT:
      case IOLINK_JOB_DS_UPLOAD:
      case IOLINK_JOB_DS_READY:
      case IOLINK_JOB_DS_CHANGE:
      case IOLINK_JOB_DS_FAULT:
      case IOLINK_JOB_OD_START:
      case IOLINK_JOB_OD_STOP:
      case IOLINK_JOB_MW_EVENT_RSP:
      case IOLINK_JOB_PERIODIC:
         if (job->callback)
         {
            job->callback (job);
         }
         job->type     = IOLINK_JOB_NONE;
         job->callback = NULL;
         os_mbox_post (master->mbox_avail, job, 0);
         break;
      case IOLINK_JOB_MW_ABORT:
      case IOLINK_JOB_MW_EVENT_REQ:

      case IOLINK_JOB_MW_PORTCONFIGURATION:
      case IOLINK_JOB_MW_READBACKPORTCONFIGURATION:
      case IOLINK_JOB_MW_PORTSTATUS:
      case IOLINK_JOB_MW_DEVICE_WRITE:
      case IOLINK_JOB_MW_DEVICE_READ:
      case IOLINK_JOB_MW_PARAM_READ:
      case IOLINK_JOB_MW_PARAM_WRITE:

         if (job->callback)
         {
            job->callback (job);
         }
         job->type     = IOLINK_JOB_NONE;
         job->callback = NULL;
         os_mbox_post (master->mbox_api_avail, job, 0);
         break;
      case IOLINK_JOB_EXIT:
         running = false;
         break;
      default:
         CC_ASSERT (0);
         break;
      }
   }

   master->has_exited = true;
}

/* Stack internal API */
iolink_job_t * iolink_fetch_avail_job (iolink_port_t * port)
{
   iolink_job_t * job;

   if (os_mbox_fetch (port->master->mbox_avail, (void **)&job, 0))
   {
	   IOL_LOG_ERR("iolink_fetch_avail_job: mbox exhausted on port %u!\n",
			   iolink_get_portnumber(port));
	   return NULL;
//      CC_ASSERT (0); // TODO: This is bad! How to continue?
   }

   job->port = port;

   return job;
}

iolink_job_t * iolink_fetch_avail_api_job (iolink_port_t * port)
{
   iolink_job_t * job;

   if (os_mbox_fetch (port->master->mbox_api_avail, (void **)&job, 0))
   {
//      CC_ASSERT (0); // TODO: This is fine, return busy
	      IOL_LOG_ERR (
	         "iolink_fetch_avail_api_job: mbox exhausted on port %u - returning busy\n",
	         iolink_get_portnumber (port));
	      return NULL;
   }

   job->port = port;

   return job;
}


void iolink_post_job_with_type_and_callback (
   iolink_port_t * port,
   iolink_job_t * job,
   iolink_job_type_t type,
   void (*callback) (struct iolink_job * job))
{
	if (job == NULL)
	{
		IOL_LOG_ERR (
				"iolink_post_job_with_type_and_callback: NULL job on port %u, dropping event type %d\n",
	         iolink_get_portnumber (port),
	         type);
	     	return;
	}
   job->type     = type;
   job->callback = callback;

   bool res = os_mbox_post (port->master->mbox, job, 0);
   if (res)
   {
      IOL_LOG_ERR (
         "iolink_post_job_with_type_and_callback: mbox post failed on port %u, type %d\n",
         iolink_get_portnumber (port),
         type);
      /* Return the job slot to the available pool so it is not lost */
      job->type     = IOLINK_JOB_NONE;
      job->callback = NULL;
      os_mbox_post (port->master->mbox_avail, job, 0);
   }
}


bool iolink_post_job_pd_event (
   iolink_port_t * port,
   uint32_t timeout,
   uint8_t data_len,
   const uint8_t * data)
{
   iolink_job_t * job;

   os_mbox_fetch (port->master->mbox_avail, (void **)&job, OS_WAIT_FOREVER);

   job->port              = port;
   job->type              = IOLINK_JOB_PD_EVENT;
   job->pd_event.data_len = data_len;
   job->pd_event.data     = data;

   uint8_t safe_len = (data_len <= IOLINK_PD_MAX_SIZE) ? data_len : IOLINK_PD_MAX_SIZE;
   memcpy (job->pd_event.data_buf, data, safe_len);
   job->pd_event.data = job->pd_event.data_buf;

   bool res = os_mbox_post (port->master->mbox, job, timeout);
   if(res){
	   IOL_LOG_ERR (
			   "iolink_post_job_pd_event: mbox post failed on port %u\n",
	         iolink_get_portnumber (port));
	      /* Return the job slot to the available pool so it is not lost */
	      job->type     = IOLINK_JOB_NONE;
	      job->callback = NULL;
	      os_mbox_post (port->master->mbox_avail, job, 0);
   }

   return res;
}

void iolink_post_job_operate_event(iolink_port_t * port)
{
    iolink_job_t * job;
    os_mbox_fetch(port->master->mbox_avail, (void **)&job, OS_WAIT_FOREVER);
    job->port     = port;
    job->type     = IOLINK_JOB_OPERATE_EVENT;
    job->callback = NULL;
    os_mbox_post(port->master->mbox, job, 0);
}

iolink_port_t * iolink_get_port (iolink_m_t * master, uint8_t portnumber)
{
   uint8_t port_index = portnumber - 1;

   if ((portnumber == 0) || (port_index > master->port_cnt))
   {
      return NULL;
   }

   return &master->ports[port_index];
}

uint8_t iolink_get_portnumber (iolink_port_t * port)
{
   return port->portnumber;
}

uint8_t iolink_get_port_cnt (iolink_port_t * port)
{
   return port->master->port_cnt;
}

iolink_port_info_t * iolink_get_port_info (iolink_port_t * port)
{
   return &port->port_info;
}

const iolink_smp_parameterlist_t * iolink_get_paramlist (iolink_port_t * port)
{
   return &port->sm.real_paramlist;
}

iolink_transmission_rate_t iolink_get_transmission_rate (iolink_port_t * port)
{
   return mhmode_to_transmission_rate (port->sm.comrate);
}

iolink_mw_port_t * iolink_get_mw_ctx (iolink_port_t * port)
{
   return &port->mw;
}


iolink_dl_t * iolink_get_dl_ctx (iolink_port_t * port)
{
   return &port->dl;
}

iolink_ds_port_t * iolink_get_ds_ctx (iolink_port_t * port)
{
   /* DS layer not integrated in this project variant */
   (void)port;
   return NULL;
}



iolink_pl_port_t * iolink_get_pl_ctx (iolink_port_t * port)
{
   return &port->pl;
}

iolink_sm_port_t * iolink_get_sm_ctx (iolink_port_t * port)
{
   return &port->sm;
}


iolink_m_t * Master_init (const iolink_m_cfg_t * m_cfg)
{
	int i;

	uint8_t port_index;

   if (iol_hw_init() != 0)
   {
      IOL_LOG_ERR ("Master_init: iol_hw_init() failed - aborting\n");
      return NULL;
   }

	const iolink_cce4511_cfg_t cfg = {0};

	iolink_hw_drv_t *hw = NULL;


	hw = iolink_cce4511_init(&cfg);

	if (hw == NULL) {
		IOL_LOG_ERR("CCE4511 REG CONFIG FAIL\r\nAPP: Failed to open driver\r\n");
		while (1)
			;
	}

	for(port_index = 0; port_index < m_cfg->port_cnt; port_index++){
		m_cfg->port_cfgs[port_index].drv = hw;
	}

   if (the_master != NULL)
   {
	   IOL_LOG_DBG ( "%s: the_master returned\r\n", __func__);
	   return the_master;
   }

   if (m_cfg->port_cnt > IOLINK_NUM_PORTS)
   {
      CC_ASSERT (m_cfg->port_cnt <= IOLINK_NUM_PORTS);
      return NULL;
   }

   size_t total =
       sizeof(iolink_m_t) +
       sizeof(iolink_port_t) * m_cfg->port_cnt;

   iolink_m_t * master = os_malloc(total);

   if (master != NULL)
   {
       memset(master, 0, total);
   }
   else
   {
	   IOL_LOG_DBG("Master Malloc failed\r\n");
       return NULL;
   }

   master->has_exited = false;

   master->port_cnt = m_cfg->port_cnt;
   master->cb_arg   = m_cfg->cb_arg;
   master->user_cb    = m_cfg->user_cb;

   for (port_index = 0; port_index < master->port_cnt; port_index++)
   {
      const iolink_port_cfg_t * port_cfg = &m_cfg->port_cfgs[port_index];

      iolink_port_t * port = &(master->ports[port_index]);

      port->master     = master;
      port->portnumber = port_index + 1;

      iolink_pl_init (port, port_cfg->drv, port_cfg->arg);
      iolink_sm_init (port);
      iolink_mw_init (port);

      iolink_sm_port_t *sm = iolink_get_sm_ctx (port);
      sm->config_paramlist.cycletime = port_cfg->cyc_time;

   }

   master->mbox 		  = os_mbox_create (IOLINK_MASTER_JOB_CNT + IOLINK_MASTER_JOB_API_CNT);
   master->mbox_avail     = os_mbox_create (IOLINK_MASTER_JOB_CNT);
   master->mbox_api_avail = os_mbox_create (IOLINK_MASTER_JOB_API_CNT);

   for (i = 0; i < IOLINK_MASTER_JOB_CNT; i++)
   {
      master->job[i].type = IOLINK_JOB_NONE;
      os_mbox_post (master->mbox_avail, &master->job[i], 0);
   }

   for (i = 0; i < IOLINK_MASTER_JOB_API_CNT; i++)
   {
      master->job_api[i].type = IOLINK_JOB_NONE;
      os_mbox_post (master->mbox_api_avail, &master->job_api[i], 0);
   }

   CC_ASSERT (master->mbox_avail != NULL);

   master->thread = os_thread_create (
      "iolink_m_thread",
      CONFIG_IOL_MASTER_TASK_PRIORITY,
      NULL,
      (uint16_t)CONFIG_IOL_MASTER_STACK_SIZE,
      iolink_main,
      master);

   CC_ASSERT (master->thread != NULL);

   for (port_index = 0; port_index < master->port_cnt; port_index++)
   {
       if (m_cfg->port_cfgs[port_index].mode != iolink_mode_INACTIVE){

    	   iolink_dl_instantiate (
    		  &master->ports[port_index],
    	      CONFIG_IOL_DL_TASK_PRIORITY,
    	      CONFIG_IOL_DL_STACK_SIZE);
       }
   }

   the_master = master;

   return master;
}

void Master_deinit (iolink_m_t ** m)
{
   int i;
   iolink_m_t * master = *m;
   iolink_job_t job;

   if (master == NULL)
   {
      return;
   }

   job.type = IOLINK_JOB_EXIT;
   if (os_mbox_post (master->mbox, &job, 0))
   {
      CC_ASSERT (0);
   }

   while (master->has_exited == false)
   {
      os_usleep (1 * 1000);
   }

   for (i = 0; i < master->port_cnt; i++)
   {
      iolink_port_t * port = &(master->ports[i]);

      os_mutex_destroy (port->mw.mtx_pdin);

   }

   os_mbox_destroy (master->mbox);
   os_mbox_destroy (master->mbox_avail);
   os_mbox_destroy (master->mbox_api_avail);

   the_master = NULL;
   free (*m);
   *m = NULL;
}

iolink_error_t Master_setPortMode(
		uint8_t portnumber,
		PortMode mode){

	if(the_master == NULL){
		return IOLINK_ERROR_MASTER_INIT;
	}

	iolink_port_t * port = iolink_get_port(the_master, portnumber);

	if (port == NULL)
	{
		return IOLINK_ERROR_PARAMETER_CONFLICT;
	}

	iolink_sm_port_t *sm = iolink_get_sm_ctx (port);
	iolink_mw_port_t *mw = iolink_get_mw_ctx(port);

	if (mode == PORT_ACTIVE)
	{
		if (mw->mw_state == MW_STATE_Inactive ||
		    mw->mw_state == MW_STATE_Recovery)
		{
			sm->config_paramlist.portnumber= portnumber;
			sm->config_paramlist.mode = IOLINK_SMTARGET_MODE_CFGCOM;
			sm->config_paramlist.revisionid = USER_REV_ID;
			sm->config_paramlist.inspectionlevel = IOLINK_INSPECTIONLEVEL_TYPE_COMP;
			sm->config_paramlist.vendorid = USER_VENDOR_ID;
			sm->config_paramlist.deviceid = USER_DEVICE_ID;
			sm->config_paramlist.functionid = USER_FUNC_ID;
			iolink_mw_event(port, MW_EVENT_Master_Port_Active);
		}
	}
	else
	{
		sm->config_paramlist.mode = IOLINK_SMTARGET_MODE_INACTIVE;
		iolink_mw_event(port, MW_EVENT_Master_Port_Inactive);
	}

	return IOLINK_ERROR_NONE;
}



iolink_error_t Master_readParameter(
		 uint8_t portnumber,
		 uint16_t index,
		 uint8_t subindex,
		 uint8_t *buf,
		 uint8_t *len){

	if (the_master == NULL)
	{
		return IOLINK_ERROR_MASTER_INIT;
	}

	if ((buf == NULL) || (len == NULL))
	{
		return IOLINK_ERROR_PARAMETER_CONFLICT;
	}

	iolink_port_t * p = iolink_get_port(the_master, portnumber);
	if (p == NULL)
	{
		return IOLINK_ERROR_PARAMETER_CONFLICT;
	}

	if ((portnumber == 0) || (portnumber > IOLINK_NUM_PORTS))
	{
		return IOLINK_ERROR_PARAMETER_CONFLICT;
	}

	iolink_mw_port_t *mw = iolink_get_mw_ctx(p);

    if (mw->mw_state != MW_STATE_Operate)
    {
		return IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE;
    }

	master_read_ctx_t * ctx = &s_master_read_ctx[portnumber - 1];
	if (ctx->in_use)
	{
		return IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE;
	}

	ctx->in_use   = true;
	ctx->ev       = os_event_create();
	ctx->res      = IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE;
	ctx->out_buf  = buf;
	ctx->out_len  = len;
	*len = 0;

	mw->service.index = index;
	mw->service.subindex = subindex;
	mw->service.direction = IOLINK_RWDIRECTION_READ;

	iolink_error_t req_err = MW_Read_req(p, index, subindex, master_read_cnf_cb);
	if (req_err != IOLINK_ERROR_NONE)
	{
		os_event_destroy(ctx->ev);
		memset(ctx, 0, sizeof(*ctx));
		return req_err;
	}

	uint32_t v = 0;
	bool timed_out = os_event_wait(ctx->ev, MASTER_RW_DONE_BIT, &v, MASTER_RW_TIMEOUT_MS);
	if (!timed_out)
	{
		os_event_clr(ctx->ev, v);
	}

	iolink_error_t res = timed_out ? IOLINK_ERROR_NO_COMM : ctx->res;

	os_event_destroy(ctx->ev);
	memset(ctx, 0, sizeof(*ctx));
	return res;
}

iolink_error_t Master_writeParameter(
		 uint8_t portnumber,
		 uint16_t index,
		 uint8_t subindex,
		 const uint8_t *buf,
		 uint8_t len){

	/* Synchronous wrapper over MW_Write_req(). */

	if (the_master == NULL)
	{
		return IOLINK_ERROR_MASTER_INIT;
	}

	if ((buf == NULL) && (len != 0))
	{
		return IOLINK_ERROR_PARAMETER_CONFLICT;
	}

	iolink_port_t * p = iolink_get_port(the_master, portnumber);
	if (p == NULL)
	{
		return IOLINK_ERROR_PARAMETER_CONFLICT;
	}

	if ((portnumber == 0) || (portnumber > IOLINK_NUM_PORTS))
	{
		return IOLINK_ERROR_PARAMETER_CONFLICT;
	}

	master_write_ctx_t * ctx = &s_master_write_ctx[portnumber - 1];
	if (ctx->in_use)
	{
		return IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE;
	}

	ctx->in_use = true;
	ctx->ev     = os_event_create();
	ctx->res    = IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE;

	iolink_error_t req_err = MW_Write_req(p, index, subindex, len, buf, master_write_cnf_cb);
	if (req_err != IOLINK_ERROR_NONE)
	{
		os_event_destroy(ctx->ev);
		memset(ctx, 0, sizeof(*ctx));
		return req_err;
	}

	uint32_t v = 0;
	bool timed_out = os_event_wait(ctx->ev, MASTER_RW_DONE_BIT, &v, MASTER_RW_TIMEOUT_MS);
	if (!timed_out)
	{
		os_event_clr(ctx->ev, v);
	}

	iolink_error_t res = timed_out ? IOLINK_ERROR_NO_COMM : ctx->res;

	os_event_destroy(ctx->ev);
	memset(ctx, 0, sizeof(*ctx));
	return res;
}


iolink_error_t Master_PDin (
   uint8_t    portnumber,
   bool     * pd_valid,
   uint8_t  * data,
   uint8_t  * len)
{
   if (the_master == NULL)
   {
      return IOLINK_ERROR_MASTER_INIT;
   }

   if ((data == NULL) || (len == NULL))
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   if ((portnumber == 0) || (portnumber > the_master->port_cnt))
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   iolink_port_t * port = iolink_get_port (the_master, portnumber);
   if (port == NULL)
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   iolink_mw_port_t * mw = iolink_get_mw_ctx (port);
   if (mw->mw_state != MW_STATE_Operate)
   {
      IOL_LOG_WRN (
         "Master_PDin: port %u MW state %u is not OPERATE\n",
         portnumber,
         mw->mw_state);
      return IOLINK_ERROR_STATE_CONFLICT;
   }

   bool valid = iolink_dl_get_pd_valid_status (port);
   if (pd_valid != NULL)
   {
      *pd_valid = valid;
   }

   os_mutex_lock (mw->mtx_pdin);
   memcpy (data, mw->pdin_data, mw->pdin_data_len);
   *len = mw->pdin_data_len;
   os_mutex_unlock (mw->mtx_pdin);

   return IOLINK_ERROR_NONE;
}

iolink_error_t Master_PDout (
   uint8_t        portnumber,
   bool           pd_valid,
   const uint8_t * data,
   uint8_t        len)
{
   if (the_master == NULL)
   {
      return IOLINK_ERROR_MASTER_INIT;
   }

   /* data may be NULL only when pd_valid=false (just marking invalid) */
   if ((data == NULL) && (len > 0))
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   if (len > IOLINK_PD_MAX_SIZE)
   {
      return IOLINK_ERROR_PDOUTLENGTH;
   }

   if ((portnumber == 0) || (portnumber > the_master->port_cnt))
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   iolink_port_t * port = iolink_get_port (the_master, portnumber);
   if (port == NULL)
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   iolink_mw_port_t * mw = iolink_get_mw_ctx (port);
   if (mw->mw_state != MW_STATE_Operate)
   {
      IOL_LOG_WRN (
         "Master_PDout: port %u MW state %u is not OPERATE\n",
         portnumber,
         mw->mw_state);
      return IOLINK_ERROR_STATE_CONFLICT;
   }

   if ((data != NULL) && (len > 0))
   {
      iolink_error_t err = MW_SetOutput_req (port, (uint8_t *)data);
      if (err != IOLINK_ERROR_NONE)
      {
         IOL_LOG_ERR (
            "Master_PDout: port %u MW_SetOutput_req failed (err=%d)\n",
            portnumber,
            (int)err);
         return err;
      }
   }

   iolink_controlcode_t ctrl = pd_valid
      ? IOLINK_CONTROLCODE_PDOUTVALID
      : IOLINK_CONTROLCODE_PDOUTINVALID;

   iolink_error_t ctrl_err = MW_Control_req (port, ctrl);
   if (ctrl_err != IOLINK_ERROR_NONE)
   {
      IOL_LOG_WRN (
         "Master_PDout: port %u MW_Control_req(%s) failed (err=%d)\n",
         portnumber,
         pd_valid ? "PDOUTVALID" : "PDOUTINVALID",
         (int)ctrl_err);
   }

   return IOLINK_ERROR_NONE;
}


iolink_error_t Master_ConfigPDFilter (
   uint8_t portnumber,
   uint8_t extract_offset,
   uint8_t extract_len)
{
   if (the_master == NULL)
   {
      return IOLINK_ERROR_MASTER_INIT;
   }

   if ((portnumber == 0) || (portnumber > the_master->port_cnt))
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   if ((extract_len > 0) && ((uint16_t)extract_offset + extract_len > IOLINK_PD_MAX_SIZE))
   {
      IOL_LOG_WRN (
         "Master_ConfigPDFilter: port %u offset+len (%u+%u) exceeds PD_MAX_SIZE (%u)\n",
         portnumber,
         extract_offset,
         extract_len,
         IOLINK_PD_MAX_SIZE);
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   iolink_port_t * port = iolink_get_port (the_master, portnumber);
   if (port == NULL)
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   MW_ConfigPDFilter (port, extract_offset, extract_len);
   return IOLINK_ERROR_NONE;
}


iolink_error_t Master_PortPowerOffOn(
    uint8_t portnumber,
    PortPowerMode powerMode,
    uint16_t PowerOffTime)
{
	if (the_master == NULL)
	{
		return IOLINK_ERROR_MASTER_INIT;
	}

	if ((portnumber == 0) || (portnumber > the_master->port_cnt))
	{
		return IOLINK_ERROR_PARAMETER_CONFLICT;
	}

	iolink_port_t * port = iolink_get_port (the_master, portnumber);
	if (port == NULL)
	{
		return IOLINK_ERROR_PARAMETER_CONFLICT;
	}
    iolink_mw_port_t * mw = iolink_get_mw_ctx (port);

    mw->port_pwr_info.offSetTime = PowerOffTime;
    mw->port_pwr_info.powerMode  = powerMode;


    switch (powerMode)
    {
    	case SwitchPowerOn:
    		if (mw->mw_state == MW_STATE_Inactive ||
    				mw->mw_state == MW_STATE_Recovery)
    		{
    			iolink_dl_reset(port);
    			iolink_mw_event(port, MW_EVENT_power_on_req);
    		}
    		break;

        case SwitchPowerOff:
        	if (mw->mw_state != MW_STATE_Inactive)
        	{
        		iolink_dl_reset(port);
        		iolink_mw_event(port, MW_EVENT_power_off_req);
        	}
        	break;

        case OneTimeSwitchOff:
        	if (mw->mw_state != MW_STATE_Inactive)
        	{
        		iolink_dl_reset(port);
        		iolink_mw_event(port, MW_EVENT_power_one_time_off);
        	}
        	break;

        default:
            return IOLINK_ERROR_PARAMETER_CONFLICT;
    }

    return IOLINK_ERROR_NONE;
}

/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/
