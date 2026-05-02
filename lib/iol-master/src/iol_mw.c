/**
  ******************************************************************************
  * @file    iol_mw.c
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    27-Mar-2026
  * @brief   This file provides functions to manage the following
  *          breif the functionalities here:
  *
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "iol-master/iol_mw.h"
#include "iol-master/iol_dl.h"
#include "iol-master/iol_main.h"
#include "iol-master/iol_sm.h"
#include "iol-master/iol_pl.h"
#include "iol-master/iol_log.h"

/**
 * @file
 * @brief Middleware layer
 *
 */

/* Bug fix: array was 10 entries (0..9) but MW_STATE_Power_Cycle=10.
 * iolink_mw_state_literals[10] read into iolink_mw_event_literals[0]
 * which is "None", producing "state None" in every error message. */
static const char * const iolink_mw_state_literals[] = {
	"Inactive_0",           /* 0  MW_STATE_Inactive */
	"Startup_1",            /* 1  MW_STATE_Startup */
	"Await_operate_ind_2",  /* 2  MW_STATE_Await_Operate */
	"Operate_3",            /* 3  MW_STATE_Operate */
	"Process_PD_4",         /* 4  MW_STATE_Process_PD */
	"Od_Exchange_5",        /* 5  MW_STATE_OD_Exchange */
	"Event_Handle_6",       /* 6  MW_STATE_Event_Handling */
	"Recovery_7",           /* 7  MW_STATE_Recovery */
	"Application_callback_8",/* 8 MW_STATE_Application_Callback */
	"Port_event_9",         /* 9  MW_STATE_Port_event */
	"Power_Cycle_10",       /* 10 MW_STATE_Power_Cycle */
	"LAST"                  /* 11 MW_STATE_LAST */
};

/* Bug fix: array was only 13 entries (NONE..Port_event) while enum has 19.
 * Reading iolink_mw_event_literals[event] for event >= 13 was undefined
 * behaviour and happened to print strings from the adjacent od_state_literals
 * array (e.g. "Await_DL_ISDU_cnf"), hiding the real event name. */
static const char * const iolink_mw_event_literals[] = {
		"None",                  /* 0  MW_EVENT_NONE */
		"Master_Port_Active",    /* 1 */
		"Master_Port_Inactive",  /* 2 */
		"SM_Port_Ind_COMREADY",  /* 3 */
		"SM_Port_Ind_OPERATE",   /* 4 */
		"NewInput_Ind",          /* 5 */
		"ISDU_Read",             /* 6  MW_EVENT_isdu_read */
		"ISDU_Write",            /* 7  MW_EVENT_isdu_write */
		"isdu_complete",         /* 8  MW_EVENT_isdu_complete */
		"event_handling",        /* 9  MW_EVENT_event_handling */
		"event_complete",        /* 10 MW_EVENT_event_complete */
		"Application_CB",        /* 11 MW_EVENT_application_cb */
		"Timeout_No_device",     /* 12 MW_EVENT_Timeout_no_device */
		"abort_port",            /* 13 MW_EVENT_abort_port */
		"power_off_req",         /* 14 MW_EVENT_power_off_req */
		"power_on_req",          /* 15 MW_EVENT_power_on_req */
		"power_one_time_off",    /* 16 MW_EVENT_power_one_time_off */
		"power_cycle_complete",  /* 17 MW_EVENT_power_cycle_complete */
		"LAST"                   /* 18 MW_EVENT_LAST */
};

typedef struct iolink_fsm_mw_transition
{
   iolink_fsm_mw_event_t event;
   iolink_mw_state_t next_state;
   iolink_fsm_mw_event_t (
      *action) (iolink_port_t * port, iolink_fsm_mw_event_t event);
} iolink_fsm_mw_transition_t;

typedef struct iolink_fsm_mw_state_transitions
{
   const uint8_t number_of_trans;
   const iolink_fsm_mw_transition_t * transitions;
} iolink_fsm_mw_state_transitions_t;

static const char * const iolink_mw_od_state_literals[] = {
   "OnReq_Idle",
   "Build_DL_Service",
   "Await_DL_param_cnf",
   "Await_DL_ISDU_cnf",
};

static const char * const iolink_mw_od_event_literals[] = {
   "NONE",
   "service",           /* T1", T16", T8 and T12 */
   "arg_err",           /* T2 */
   "param_read",        /* T3 */
   "param_write_1",     /* T4 */
   "param_write_2",     /* T5 */
   "isdu_read",         /* T6 */
   "isdu_write",        /* T7 */
   "abort",             /* T9 and T11 */
   "rwparam",           /* T10 */
   "readparam_cnf",     /* T13 */
   "writeparam_cnf",    /* T14 */
   "isdutransport_cnf", /* T15 */
   "abort_port",        /* T17 */
};

static const char * const iolink_mw_event_state_literals[] = {
   "Event_inactive",
   "Event_idle",
   "Read_Event_Set",
   "DU_Event_handling",
   "LAST",
};

static const char * const iolink_mw_event_event_literals[] = {
   "NONE",
   "dl_event_ind_more", /* T3 or T4 */
   "dl_event_ind_done", /* T5 or T3 + T5 */
   "mw_event_rsp",      /* T6 */
   "mw_event_req",      /* T7 */
   "LAST",
};

typedef struct iolink_fsm_mw_od_transition
{
   iolink_fsm_mw_od_event_t event;
   iolink_mw_od_state_t next_state;
   iolink_fsm_mw_od_event_t (
      *action) (iolink_port_t * port, iolink_fsm_mw_od_event_t event);
} iolink_fsm_mw_od_transition_t;

typedef struct iolink_fsm_mw_od_state_transitions
{
   const uint8_t number_of_trans;
   const iolink_fsm_mw_od_transition_t * transitions;
} iolink_fsm_mw_od_state_transitions_t;

typedef struct iolink_fsm_mw_event_transition
{
   iolink_fsm_mw_event_event_t event;
   iolink_mw_event_state_t next_state;
   iolink_fsm_mw_event_event_t (
      *action) (iolink_port_t * port, iolink_fsm_mw_event_event_t event);
} iolink_fsm_mw_event_transition_t;

typedef struct iolink_fsm_mw_event_state_transitions
{
   const uint8_t number_of_trans;
   const iolink_fsm_mw_event_transition_t * transitions;
} iolink_fsm_mw_event_state_transitions_t;


/* Declare prototypes of static functions as to comply with MISRA-071_a-3 */
static iolink_error_t mw_err_to_errortype (
   iolink_mw_port_t * al,
   bool is_isdu);
static iolink_fsm_mw_od_event_t mw_od_busy (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static iolink_fsm_mw_od_event_t mw_od_build_dl_service (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static iolink_fsm_mw_od_event_t mw_od_send_mw_cnf (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static iolink_fsm_mw_od_event_t mw_od_send_abort_mw_cnf (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static iolink_fsm_mw_od_event_t mw_od_isdu_read_write (
   iolink_port_t * port,
   iolink_rwdirection_t rwdirection);
static iolink_fsm_mw_od_event_t mw_od_isdu_read (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static iolink_fsm_mw_od_event_t mw_od_isdu_write (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static iolink_fsm_mw_od_event_t mw_od_param_read (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static iolink_fsm_mw_od_event_t mw_od_param_write (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static iolink_fsm_mw_od_event_t mw_od_param_next_rw (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static void iolink_mw_od_event (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event);
static iolink_fsm_mw_event_event_t mw_event_wait_read (
   iolink_port_t * port,
   iolink_fsm_mw_event_event_t event);
static iolink_fsm_mw_event_event_t mw_event_du_handle (
   iolink_port_t * port,
   iolink_fsm_mw_event_event_t event);
static iolink_fsm_mw_event_event_t mw_event_idle (
   iolink_port_t * port,
   iolink_fsm_mw_event_event_t event);
static void iolink_mw_event_event (
   iolink_port_t * port,
   iolink_fsm_mw_event_event_t event);
static void mw_read_write_cb (iolink_job_t * job);
static void mw_dl_event_ind_cb (iolink_job_t * job);
static void dl_control_ind_cb (iolink_job_t * job);
static void mw_dl_readparam_cnf_cb (iolink_job_t * job);
static void mw_dl_writeparam_cnf_cb (iolink_job_t * job);
static void mw_dl_isdu_transport_cnf_cb (iolink_job_t * job);
static void mw_abort_cb (iolink_job_t * job);
static void mw_event_rsp_cb (iolink_job_t * job);
static void mw_event_req_cb (iolink_job_t * job);


static iolink_fsm_mw_event_t mw_action_nop(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
	    return MW_EVENT_NONE;
}

static iolink_fsm_mw_event_t mw_action_inactive(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
    iolink_smp_parameterlist_t paraml;
	iolink_sm_port_t *sm = iolink_get_sm_ctx (port);
	memcpy(&paraml, &sm->config_paramlist, sizeof(paraml));
    SM_SetPortConfig_req(port, &paraml);
    return MW_EVENT_NONE;
}

static iolink_fsm_mw_event_t mw_action_start_startup(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
    iolink_smp_parameterlist_t paraml;
	iolink_sm_port_t *sm = iolink_get_sm_ctx (port);
	memcpy(&paraml, &sm->config_paramlist, sizeof(paraml));
    SM_SetPortConfig_req(port, &paraml);
    return MW_EVENT_NONE;
}

static iolink_fsm_mw_event_t mw_action_comready(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
    SM_Operate(port);
    return MW_EVENT_NONE;
}

static iolink_fsm_mw_event_t mw_action_port_up(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
    iolink_mw_port_t * mw = iolink_get_mw_ctx(port);
    mw->event_state = MW_EVENT_STATE_Event_idle;
    iolink_post_job_operate_event(port);
    return MW_EVENT_NONE;
}

static iolink_fsm_mw_event_t mw_action_deactivate(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
    iolink_mw_port_t * mw = iolink_get_mw_ctx(port);
    if (mw->od_state != MW_OD_STATE_OnReq_Idle)
    {
        mw->service.errinfo = IOLINK_STATUS_NO_COMM;
        iolink_mw_od_event(port, MW_OD_EVENT_abort);
    }
    mw->event_state = MW_EVENT_STATE_Event_inactive;
    return MW_EVENT_NONE;
}

static os_timer_t * s_power_cycle_timer[IOLINK_NUM_PORTS];
static void power_cycle_complete_cb(iolink_job_t * job)
{
    iolink_mw_event(job->port, MW_EVENT_power_cycle_complete);
}

static os_timer_t * s_recovery_timer[IOLINK_NUM_PORTS];

static void power_cycle_timer_cb(os_timer_t *timer, void *arg)
{
    iolink_port_t * port = (iolink_port_t *)arg;
    IOL_LOG_INF("MW (%u): power cycle timer expired - restarting\n",
                iolink_get_portnumber(port));

    iolink_job_t * job = iolink_fetch_avail_job(port);
    if (job)
    {
        iolink_post_job_with_type_and_callback(port, job,
            IOLINK_JOB_PERIODIC,
            power_cycle_complete_cb);
    }
    uint8_t pidx = iolink_get_portnumber(port) - 1;
    if (s_recovery_timer[pidx]) {
        os_timer_stop(s_recovery_timer[pidx]);
    }
}


static iolink_fsm_mw_event_t mw_action_recovery(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
    mw_action_deactivate(port, event);
    iolink_pl_port_power_off(port);

    os_usleep(100000);

    iolink_pl_port_power_on(port);
    os_usleep(100000);

    iolink_dl_reset(port);
    IOL_LOG_WRN("MW (%u): port recovery\n", iolink_get_portnumber(port));

    uint8_t pidx = iolink_get_portnumber(port) - 1;
    if (s_recovery_timer[pidx] == NULL) {
        s_recovery_timer[pidx] = os_timer_create(
        	3000000,
            power_cycle_timer_cb, port, false);
    }
    os_timer_start(s_recovery_timer[pidx]);
    return MW_EVENT_NONE;
}

static iolink_fsm_mw_event_t mw_action_process_pd(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
    iolink_mw_port_t * mw = iolink_get_mw_ctx (port);

    os_mutex_lock (mw->mtx_pdin);

    uint8_t raw_len = mw->pdin_data_len;
    uint8_t offset  = mw->pd_extract_offset;
    uint8_t extract = mw->pd_extract_len;

    if ((extract == 0) ||
        (extract > raw_len) ||
        ((uint16_t)offset + extract) > raw_len)
    {
        memcpy (mw->pd_filtered, mw->pdin_data, raw_len);
        mw->pd_filtered_len = raw_len;

        if (extract != 0)
        {
            IOL_LOG_WRN (
                "MW (%u): PD filter  (offset=%u len=%u raw=%u), passing all\n",
                iolink_get_portnumber (port),
                offset, extract, raw_len);
        }
    }
    else
    {
        memcpy (mw->pd_filtered, &mw->pdin_data[offset], extract);
        mw->pd_filtered_len = extract;
    }

    os_mutex_unlock (mw->mtx_pdin);

    return MW_EVENT_application_cb;
}

static iolink_fsm_mw_event_t mw_action_app_callback(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
    iolink_mw_port_t * mw = iolink_get_mw_ctx (port);

    bool fail = iolink_post_job_pd_event(
        port,
        100,
        mw->pd_filtered_len,
        mw->pd_filtered);

    if (fail) {
        IOL_LOG_ERR("MW (%u): Failed to post\n",
                    iolink_get_portnumber(port));
    }

    return MW_EVENT_NONE;
}


static iolink_fsm_mw_event_t mw_action_port_power(
    iolink_port_t * port, iolink_fsm_mw_event_t event)
{
    uint8_t pidx = iolink_get_portnumber(port) - 1;

    iolink_mw_port_t * mw = iolink_get_mw_ctx (port);

    switch (mw->port_pwr_info.powerMode)
    {
        case OneTimeSwitchOff:
            iolink_dl_reset(port);
            if (s_power_cycle_timer[pidx] == NULL)
            {
                s_power_cycle_timer[pidx] =
                    os_timer_create(mw->port_pwr_info.offSetTime,
                                    power_cycle_timer_cb,
                                    port,
                                    false);
            }
            os_timer_start(s_power_cycle_timer[pidx]);
            IOL_LOG_INF("MW (%u): OneTimeSwitchOff - off for %u ms\n",
                        iolink_get_portnumber(port),
                        mw->port_pwr_info.offSetTime);
            break;

        case SwitchPowerOn:
        case SwitchPowerOff:
            break;
    }

    return MW_EVENT_NONE;
}
/* MW FSM state transitions */

static const iolink_fsm_mw_transition_t mw_trans_s0[] = {
    /* Inactive_0 */
    {MW_EVENT_Master_Port_Active,    MW_STATE_Startup,  mw_action_start_startup},
    {MW_EVENT_Master_Port_Inactive,  MW_STATE_Inactive, mw_action_inactive},
    {MW_EVENT_power_cycle_complete,  MW_STATE_Startup,  mw_action_start_startup},
    {MW_EVENT_power_on_req,          MW_STATE_Startup,  mw_action_start_startup},
};

static const iolink_fsm_mw_transition_t mw_trans_s1[] = {
    {MW_EVENT_SM_Port_COMREADY,     MW_STATE_Await_Operate,   mw_action_comready},
    {MW_EVENT_Timeout_no_device,    MW_STATE_Recovery,        mw_action_recovery},
    {MW_EVENT_Master_Port_Inactive, MW_STATE_Inactive,        mw_action_deactivate},
};

static const iolink_fsm_mw_transition_t mw_trans_s3[] = {
    /* Await_Operate_ind_3*/
    {MW_EVENT_SM_Port_OPERATE,      MW_STATE_Operate,         mw_action_port_up},
    {MW_EVENT_Master_Port_Inactive,  MW_STATE_Inactive,       mw_action_deactivate},
};

static const iolink_fsm_mw_transition_t mw_trans_s4[] = {
    /* Operate_4 */
    {MW_EVENT_NewInput_Ind,          MW_STATE_Process_PD,    mw_action_process_pd},
    {MW_EVENT_isdu_read,             MW_STATE_OD_Exchange,   mw_action_nop},
    {MW_EVENT_isdu_write,            MW_STATE_OD_Exchange,   mw_action_nop},
    {MW_EVENT_event_handling,        MW_STATE_Event_Handling,mw_action_nop},
    {MW_EVENT_Timeout_no_device,     MW_STATE_Recovery,      mw_action_recovery},
    {MW_EVENT_Master_Port_Inactive,  MW_STATE_Recovery,      mw_action_recovery},
    {MW_EVENT_power_one_time_off,    MW_STATE_Power_Cycle,   mw_action_port_power},
    {MW_EVENT_power_off_req,         MW_STATE_Recovery,      mw_action_port_power},
    {MW_EVENT_power_cycle_complete,  MW_STATE_Startup,         mw_action_start_startup},

};

static const iolink_fsm_mw_transition_t mw_trans_s5[] = {
    /* Process_PD_5 */
	{MW_EVENT_application_cb,        MW_STATE_Operate,      mw_action_app_callback},
	{MW_EVENT_Master_Port_Inactive,  MW_STATE_Recovery,     mw_action_recovery},
	{MW_EVENT_power_one_time_off,    MW_STATE_Power_Cycle,  mw_action_port_power},
	{MW_EVENT_power_off_req,         MW_STATE_Recovery,     mw_action_port_power},
};

static const iolink_fsm_mw_transition_t mw_trans_s6[] = {
    /* OD_Exchange_6 */
    {MW_EVENT_isdu_complete,         MW_STATE_Operate,         mw_action_nop},
    {MW_EVENT_abort_port,            MW_STATE_Operate,         mw_action_nop},
    {MW_EVENT_NewInput_Ind,          MW_STATE_Process_PD,     mw_action_process_pd},
    {MW_EVENT_Master_Port_Inactive,  MW_STATE_Recovery,        mw_action_recovery},
};

static const iolink_fsm_mw_transition_t mw_trans_s7[] = {
    /* Event_Handling_7 */
    {MW_EVENT_event_complete,        MW_STATE_Operate,         mw_action_nop},
    {MW_EVENT_Master_Port_Inactive,  MW_STATE_Recovery,        mw_action_recovery},
};

static const iolink_fsm_mw_transition_t mw_trans_s8[] = {
    /* Recovery_8 */
    {MW_EVENT_Master_Port_Active,   MW_STATE_Startup,  mw_action_start_startup},
    {MW_EVENT_Master_Port_Inactive, MW_STATE_Inactive, mw_action_deactivate},
    {MW_EVENT_power_on_req,         MW_STATE_Startup,  mw_action_start_startup},
	{MW_EVENT_Timeout_no_device, MW_STATE_Recovery,    mw_action_nop},
    {MW_EVENT_power_cycle_complete,  MW_STATE_Startup, mw_action_start_startup},

};
static const iolink_fsm_mw_transition_t mw_trans_s9[] = {
    {MW_EVENT_application_cb, MW_STATE_Operate, mw_action_app_callback},
};

static const iolink_fsm_mw_transition_t mw_trans_s_pwrcycle[] = {
    {MW_EVENT_power_cycle_complete,  MW_STATE_Startup,      mw_action_start_startup},
    {MW_EVENT_Master_Port_Inactive,  MW_STATE_Power_Cycle,  mw_action_nop},
    {MW_EVENT_Timeout_no_device,     MW_STATE_Power_Cycle,  mw_action_nop},
    {MW_EVENT_Master_Port_Active,    MW_STATE_Power_Cycle,  mw_action_nop},
};

/* The index is the state in this array */
static const iolink_fsm_mw_state_transitions_t iolink_mw_fsm[] = {
		{/* Inactive_0 */
				.number_of_trans = NELEMENTS (mw_trans_s0),
				.transitions     = mw_trans_s0},
		{/* Startup_2 */
				.number_of_trans = NELEMENTS (mw_trans_s1),
				.transitions     = mw_trans_s1},
		{/* Await_Operate_Ind_3 */
				.number_of_trans = NELEMENTS (mw_trans_s3),
				.transitions     = mw_trans_s3},
		{/* Operate_4 */
				.number_of_trans = NELEMENTS (mw_trans_s4),
				.transitions     = mw_trans_s4},
		{/* Process_pd_5 */
				.number_of_trans = NELEMENTS (mw_trans_s5),
				.transitions     = mw_trans_s5},
		{/* OD_Exchange_6 */
				.number_of_trans = NELEMENTS (mw_trans_s6),
				.transitions     = mw_trans_s6},
		{/* Event_handling_7 */
				.number_of_trans = NELEMENTS (mw_trans_s7),
				.transitions     = mw_trans_s7},
		{/* Recovery_8 */
				.number_of_trans = NELEMENTS (mw_trans_s8),
				.transitions     = mw_trans_s8},
		{/* Application_Callback_9 - MW_STATE_Application_Callback=8 */
				.number_of_trans = NELEMENTS (mw_trans_s9),
		    	.transitions     = mw_trans_s9},
		{/* Port_event_9 - */
				.number_of_trans = 0,
		    	.transitions     = NULL},
		{/* Power_Cycle_10 - MW_STATE_Power_Cycle=10 */
			    .number_of_trans = NELEMENTS(mw_trans_s_pwrcycle),
			    .transitions     = mw_trans_s_pwrcycle},
};

/**
 * Trigger MW FSM state transition
 *
 * This function triggers an MW state transition according to the event.
 * @param port          Port handle
 * @param event         MW event
 */
void iolink_mw_event (iolink_port_t * port, iolink_fsm_mw_event_t event)
{
   do
   {
      int i;
      iolink_mw_port_t * mw        = iolink_get_mw_ctx (port);
      iolink_mw_state_t previous = mw->mw_state;
      const iolink_fsm_mw_transition_t * next_trans = NULL;

      for (i = 0; i < iolink_mw_fsm[previous].number_of_trans; i++)
      {
         if (iolink_mw_fsm[previous].transitions[i].event == event)
         {
            if (!next_trans)
            {
               next_trans = &iolink_mw_fsm[previous].transitions[i];
               break;
            }
         }
      }
      if (!next_trans)
      {
         IOL_LOG_ERR (
            "MW (%u): next_trans == NULL: state %s - event %s\n",
            iolink_get_portnumber (port),
            iolink_mw_state_literals[previous],
            iolink_mw_event_literals[event]);
         return;
      }

      /* Transition to next state */
      mw->mw_state = next_trans->next_state;

//      IOL_LOG_DBG (
//         "MW (%u): event: %s, state transition: %s -> %s\n",
//         iolink_get_portnumber (port),
//         iolink_mw_event_literals[event],
//         iolink_mw_state_literals[previous],
//         iolink_mw_state_literals[mw->mw_state]);

      event = next_trans->action (port, event);
   } while (event != MW_EVENT_NONE);
}

static iolink_error_t mw_err_to_errortype (
   iolink_mw_port_t * al,
   bool is_isdu)
{
   iolink_rwdirection_t direction = al->service.direction;
   iolink_status_t stat           = al->service.errinfo;
   iservice_t qualifier           = al->service.qualifier;

   iolink_error_t errortype = IOLINK_ERROR_NONE;

   if (is_isdu)
   {
      if (stat == IOLINK_STATUS_ISDU_CHECKSUM_ERROR)
      {
         errortype = IOLINK_ERROR_ISDU_CHECKSUM_ERROR;
      }
      else if (direction == IOLINK_RWDIRECTION_READ)
      {
         switch (qualifier)
         {
         case IOL_ISERVICE_DEVICE_READ_RESPONSE_NEG:
         {
            if (al->service.data_len == 2)
            {
               errortype = al->service.data_read[0] << 8 |
                           al->service.data_read[1];
            }
            else
            {
               errortype = IOLINK_ERROR_DEV_APPLICATION; // TODO how to know
                                                         // error type?
            }
            break;
         }
         case IOL_ISERVICE_DEVICE_READ_RESPONSE_POS:
            break; /* ISDU read ok */
         default:
            IOL_LOG_ERR (
               "AL: ISDU_read_cnf: Got unexpected qualifier 0x%02x\n",
               qualifier);
            errortype = IOLINK_ERROR_DEV_APPLICATION;
            break;
         }
      }
      else if (direction == IOLINK_RWDIRECTION_WRITE)
      {
         switch (qualifier)
         {
         case IOL_ISERVICE_DEVICE_WRITE_RESPONSE_NEG:
         {
            if (al->service.data_len == 2)
            {
               errortype = al->service.data_read[0] << 8 |
                           al->service.data_read[1];
            }
            else
            {
               errortype = IOLINK_ERROR_DEV_APPLICATION; // TODO how to know
                                                         // error type?
            }
            break;
         }
         case IOL_ISERVICE_DEVICE_WRITE_RESPONSE_POS:
            break; /* ISDU write ok */
         case IOL_ISERVICE_DEVICE_NO_SERVICE:
            if (stat == IOLINK_STATUS_NO_ERROR)
            {
               errortype = IOLINK_ERROR_NO_COMM;
            }
            break;
         default:
            IOL_LOG_DBG (
               "AL: ISDU_write_cnf: Got unexpected qualifier 0x%02x\n",
               qualifier);
            break;
         }
      }
   }

   return errortype;
}

static iolink_fsm_mw_od_event_t mw_od_busy (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event)
{
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);
   iolink_job_t * job    = al->service.job_req;

   switch (job->type)
   {
   case IOLINK_JOB_MW_READ_REQ:
      job->mw_read_req.mw_read_cb (
         port,
         0,
         NULL,
		 IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE);
      break;
   case IOLINK_JOB_MW_WRITE_REQ:
      job->mw_write_req.mw_write_cb (
         port,
		 IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE);
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   return MW_OD_EVENT_NONE;
}

static iolink_fsm_mw_od_event_t mw_od_build_dl_service (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event)
{
   iolink_mw_port_t * al              = iolink_get_mw_ctx (port);
   iolink_job_t * job                 = al->service.job_req;
   uint16_t index                     = 0;
   bool isdu_flag                     = true; // TODO where do we get this from?
   iolink_fsm_mw_od_event_t res_event = MW_OD_EVENT_arg_err;

   switch (job->type)
   {
   case IOLINK_JOB_MW_READ_REQ:
      al->service.data_len = 0;
      index = al->service.index  = job->mw_read_req.index;
      al->service.subindex       = job->mw_read_req.subindex;
      al->service.direction      = IOLINK_RWDIRECTION_READ;
      al->service.mw_read_cnf_cb = job->mw_read_req.mw_read_cb;

      if (index <= 1)
      {
         res_event = MW_OD_EVENT_param_read;
      }
      else if (index >= 2 && isdu_flag)
      {
         res_event = MW_OD_EVENT_isdu_read;
      }
      break;
   case IOLINK_JOB_MW_WRITE_REQ:
      index = al->service.index   = job->mw_write_req.index;
      al->service.subindex        = job->mw_write_req.subindex;
      al->service.data_len        = job->mw_write_req.length;
      al->service.direction       = IOLINK_RWDIRECTION_WRITE;
      al->service.data_write      = job->mw_write_req.data;
      al->service.mw_write_cnf_cb = job->mw_write_req.mw_write_cb;

      if (index == 1)
      {
         res_event = MW_OD_EVENT_param_write_1;
      }
      else if ((index == 2) && !isdu_flag)
      {
         res_event = MW_OD_EVENT_param_write_2;
      }
      else if ((index >= 2) && isdu_flag)
      {
         res_event = MW_OD_EVENT_isdu_write;
      }
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   if (res_event == MW_OD_EVENT_arg_err)
   {
      al->service.errinfo = IOLINK_STATUS_PARAMETER_CONFLICT;
   }

   return res_event;
}

static iolink_fsm_mw_od_event_t mw_od_send_mw_cnf (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event)
{
   iolink_mw_port_t * al             = iolink_get_mw_ctx (port);
   iolink_error_t errortype = IOLINK_ERROR_NONE;

   switch (event)
   {
   case MW_OD_EVENT_abort: /* T9 and T11, + T16 */
      // TODO All current DL service actions are abandoned.... -> and a negative
      // AL service confirmation is prepared
      /* fall through */
   case MW_OD_EVENT_arg_err: /* T2 + T16 */
      errortype = mw_err_to_errortype (al, false);

      /* Prepare negative AL service confirmation */
      if (al->service.direction == IOLINK_RWDIRECTION_READ)
      {
         al->service.mw_read_cnf_cb (port, 0, NULL, errortype);
      }
      else if (al->service.direction == IOLINK_RWDIRECTION_WRITE)
      {
         al->service.mw_write_cnf_cb (port, errortype);
      }
      break;
   case MW_OD_EVENT_readparam_cnf: /* T13 + T16 */
      errortype = mw_err_to_errortype (al, false);
      al->service.mw_read_cnf_cb (
         port,
         al->service.data_len,
         al->service.data_read,
         errortype);
      break;
   case MW_OD_EVENT_isdutransport_cnf: /* T15 + T16 */
   {
      errortype = mw_err_to_errortype (al, true);

      if (al->service.direction == IOLINK_RWDIRECTION_READ)
      {
         al->service.mw_read_cnf_cb (
            port,
            al->service.data_len,
            al->service.data_read,
            errortype);
      }
      else if (al->service.direction == IOLINK_RWDIRECTION_WRITE)
      {
         al->service.mw_write_cnf_cb (port, errortype);
      }
      break;
   }
   case MW_OD_EVENT_writeparam_cnf: /* T14 + T16 */
      errortype = mw_err_to_errortype (al, false);
      al->service.mw_write_cnf_cb (port, errortype);
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   return MW_OD_EVENT_NONE;
}

static iolink_fsm_mw_od_event_t mw_od_send_abort_mw_cnf (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event)
{
   if (event == MW_OD_EVENT_abort_port) /* T17 */
   {
      // TODO send negative response for portx MW_Read_cnf/MW_Write_cnf!?
   }

   return MW_OD_EVENT_NONE;
}

static iolink_fsm_mw_od_event_t mw_od_isdu_read_write (
   iolink_port_t * port,
   iolink_rwdirection_t rwdirection)
{
   iolink_isdu_vl_t valuelist;
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   valuelist.index     = al->service.index;
   valuelist.subindex  = al->service.subindex;
   valuelist.readwrite = rwdirection;

   if (rwdirection == IOLINK_RWDIRECTION_READ)
   {
      valuelist.data_read = al->service.data_read;
   }
   else
   {
      valuelist.data_write = al->service.data_write;
   }
   valuelist.length = al->service.data_len;

   if (DL_ISDUTransport_req (port, &valuelist) != IOLINK_ERROR_NONE)
   {
      IOL_LOG_ERR (
         "AL: bad %s DL_ISDUTransport_req!\n",
         (rwdirection == IOLINK_RWDIRECTION_READ) ? "Read" : "Write");
   }

   return MW_OD_EVENT_NONE;
}

static iolink_fsm_mw_od_event_t mw_od_isdu_read (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event)
{
   return mw_od_isdu_read_write (port, IOLINK_RWDIRECTION_READ);
}

static iolink_fsm_mw_od_event_t mw_od_isdu_write (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event)
{
   return mw_od_isdu_read_write (port, IOLINK_RWDIRECTION_WRITE);
}

static iolink_fsm_mw_od_event_t mw_od_param_read (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event)
{
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   CC_ASSERT (al != NULL);

   DL_ReadParam_req (port, al->service.index);

   return MW_OD_EVENT_NONE;
}

static iolink_fsm_mw_od_event_t mw_od_param_write (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event)
{
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   // TODO should we write multiple bytes?
   switch (event)
   {
   case MW_OD_EVENT_param_write_1: /* T4 */
      CC_ASSERT (al->service.index == 1);
      break;
   case MW_OD_EVENT_param_write_2: /* T5 */
      CC_ASSERT (al->service.index == 2);
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   DL_WriteParam_req (port, al->service.index, al->service.data_write[0]);

   return MW_OD_EVENT_NONE;
}

static iolink_fsm_mw_od_event_t mw_od_param_next_rw (
   iolink_port_t * port,
   iolink_fsm_mw_od_event_t event)
{
   // TODO Call next DL_ReadParam or DL_WriteParam service, if not all OD are
   // transferred
   return MW_OD_EVENT_NONE;
}



/* AL OD FSM state transitions, IO-Link Interface Spec v1.1.3 Chapter 8.3.2.1 */
/* since we iterate through the list on events put the most likely in the top of
 * the list. */
static const iolink_fsm_mw_od_transition_t mw_od_trans_s0[] = {
   /* OnReq_Idle_0 */
   {MW_OD_EVENT_service, MW_OD_STATE_Build_DL_Service, mw_od_build_dl_service}, /* T1 */
   {MW_OD_EVENT_abort_port, MW_OD_STATE_OnReq_Idle, mw_od_send_abort_mw_cnf}, /* T17 */
};

static const iolink_fsm_mw_od_transition_t mw_od_trans_s1[] = {
   /* Build_DL_Service_1 */
   {MW_OD_EVENT_arg_err, MW_OD_STATE_OnReq_Idle, mw_od_send_mw_cnf}, /* T2 + T16
                                                                      */
   {MW_OD_EVENT_param_read, MW_OD_STATE_Await_DL_param_cnf, mw_od_param_read}, /* T3 */
   {MW_OD_EVENT_param_write_1,
    MW_OD_STATE_Await_DL_param_cnf,
    mw_od_param_write}, /* T4 */
   {MW_OD_EVENT_param_write_2,
    MW_OD_STATE_Await_DL_param_cnf,
    mw_od_param_write}, /* T5 */
   {MW_OD_EVENT_isdu_read, MW_OD_STATE_Await_DL_ISDU_cnf, mw_od_isdu_read}, /* T6
                                                                             */
   {MW_OD_EVENT_isdu_write, MW_OD_STATE_Await_DL_ISDU_cnf, mw_od_isdu_write}, /* T7 */
};

static const iolink_fsm_mw_od_transition_t mw_od_trans_s2[] = {
   /* Await_DL_Param_cnf_2 */
   {MW_OD_EVENT_service, MW_OD_STATE_Await_DL_param_cnf, mw_od_busy}, /* T8 */
   {MW_OD_EVENT_abort, MW_OD_STATE_OnReq_Idle, mw_od_send_mw_cnf}, /* T9 + T16
                                                                    */
   {MW_OD_EVENT_rwparam, MW_OD_STATE_Await_DL_param_cnf, mw_od_param_next_rw}, /* T10 */
   {MW_OD_EVENT_readparam_cnf, MW_OD_STATE_OnReq_Idle, mw_od_send_mw_cnf}, /* T13
                                                                              +
                                                                              T16
                                                                            */
   {MW_OD_EVENT_writeparam_cnf, MW_OD_STATE_OnReq_Idle, mw_od_send_mw_cnf}, /* T14
                                                                               + T16 */
};

static const iolink_fsm_mw_od_transition_t mw_od_trans_s3[] = {
   /* Await_DL_ISDU_cnf_3 */
   {MW_OD_EVENT_abort, MW_OD_STATE_OnReq_Idle, mw_od_send_mw_cnf}, /* T11 + T16
                                                                    */
   {MW_OD_EVENT_service, MW_OD_STATE_Await_DL_ISDU_cnf, mw_od_busy}, /* T12 */
   {MW_OD_EVENT_isdutransport_cnf,
    MW_OD_STATE_OnReq_Idle,
    mw_od_send_mw_cnf}, /* T15 + T16 */
};

/* The index is the state in this array */
static const iolink_fsm_mw_od_state_transitions_t iolink_mw_od_fsm[] = {
   {/* OnReq_Idle_0 */
    .number_of_trans = NELEMENTS (mw_od_trans_s0),
    .transitions     = mw_od_trans_s0},
   {/* Build_DL_Service_1 */
    .number_of_trans = NELEMENTS (mw_od_trans_s1),
    .transitions     = mw_od_trans_s1},
   {/* Await_DL_Param_cnf_2 */
    .number_of_trans = NELEMENTS (mw_od_trans_s2),
    .transitions     = mw_od_trans_s2},
   {/* Await_DL_ISDU_cnf_3 */
    .number_of_trans = NELEMENTS (mw_od_trans_s3),
    .transitions     = mw_od_trans_s3},
};

/**
 * Trigger AL OD FSM state transition
 *
 * This function triggers an AL OD state transition according to the event.
 *
 * @param port          Port handle
 * @param event         AL OD event
 */
static void iolink_mw_od_event (iolink_port_t * port, iolink_fsm_mw_od_event_t event)
{
   do
   {
      int i;
      iolink_mw_port_t * al         = iolink_get_mw_ctx (port);
      iolink_mw_od_state_t previous = al->od_state;
      const iolink_fsm_mw_od_transition_t * next_trans = NULL;

      for (i = 0; i < iolink_mw_od_fsm[previous].number_of_trans; i++)
      {
         if (iolink_mw_od_fsm[previous].transitions[i].event == event)
         {
            if (!next_trans)
            {
               next_trans = &iolink_mw_od_fsm[previous].transitions[i];
               break;
            }
         }
      }
      if (!next_trans)
      {
         IOL_LOG_ERR (
            "AL OD (%u): next_trans == NULL: state %s - event %s\n",
            iolink_get_portnumber (port),
            iolink_mw_od_state_literals[previous],
            iolink_mw_od_event_literals[event]);
         return;
      }

      /* Transition to next state */
      al->od_state = next_trans->next_state;

      IOL_LOG_DBG (
         "AL OD (%u): event: %s, state transition: %s -> %s\n",
         iolink_get_portnumber (port),
         iolink_mw_od_event_literals[event],
         iolink_mw_od_state_literals[previous],
         iolink_mw_od_state_literals[al->od_state]);

      event = next_trans->action (port, event);
   } while (event != MW_OD_EVENT_NONE);
}

static iolink_fsm_mw_event_event_t mw_event_wait_read (
   iolink_port_t * port,
   iolink_fsm_mw_event_event_t event)
{
   /* Wait for more data */
   return MW_EVENT_EVENT_NONE;
}

static iolink_fsm_mw_event_event_t mw_event_du_handle (
   iolink_port_t * port,
   iolink_fsm_mw_event_event_t event)
{
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   MW_Event_ind (port, al->event.event_cnt, al->event.events);
   /* Now wait for diagnosis unit to acknowledge, with MW_Event_rsp() */
   return MW_EVENT_EVENT_NONE;
}

static iolink_fsm_mw_event_event_t mw_event_idle (
   iolink_port_t * port,
   iolink_fsm_mw_event_event_t event)
{
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   switch (event)
   {
   case MW_EVENT_EVENT_mw_event_rsp:
      /* Event is confirmed in DL */
      break;
   case MW_EVENT_EVENT_mw_event_req:
      // TODO call MW_Event_ind (port, ???);
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   /* Ready to accept DL_Events from DL */
   al->event.event_cnt = 0;

   return MW_EVENT_EVENT_NONE;
}

/* AL Event FSM state transitions, IO-Link Interface Spec v1.1.3 Chapter 8.3.3.1
 */
/* since we iterate through the list on events put the most likely in the top of
 * the list. */
static const iolink_fsm_mw_event_transition_t mw_event_trans_s0[] = {
   /* Event_idle_1 */
   {MW_EVENT_EVENT_dl_event_ind_more,
    MW_EVENT_STATE_Read_Event_Set,
    mw_event_wait_read}, /* T3 */
   {MW_EVENT_EVENT_dl_event_ind_done,
    MW_EVENT_STATE_DU_Event_handling,
    mw_event_du_handle}, /* T3 + T5 */
   {MW_EVENT_EVENT_mw_event_req, MW_EVENT_STATE_Event_idle, mw_event_idle}, /* T7
                                                                             */
};

static const iolink_fsm_mw_event_transition_t mw_event_trans_s1[] = {
   /* Read_Event_Set_2 */
   {MW_EVENT_EVENT_dl_event_ind_more,
    MW_EVENT_STATE_Read_Event_Set,
    mw_event_wait_read}, /* T4 */
   {MW_EVENT_EVENT_dl_event_ind_done,
    MW_EVENT_STATE_DU_Event_handling,
    mw_event_du_handle}, /* T5 */
};

static const iolink_fsm_mw_event_transition_t mw_event_trans_s2[] = {
   /* DU_Event_handling_3 */
   {MW_EVENT_EVENT_mw_event_rsp, MW_EVENT_STATE_Event_idle, mw_event_idle}, /* T6
                                                                             */
};

/* The index is the state in this array */
static const iolink_fsm_mw_event_state_transitions_t iolink_mw_event_fsm[] = {

   {/* Event_Inactive_0 */
    .number_of_trans = 0,
    .transitions     = NULL},
   {/* Event_idle_1 */
    .number_of_trans = NELEMENTS (mw_event_trans_s0),
    .transitions     = mw_event_trans_s0},
   {/* Read_Event_Set_2 */
    .number_of_trans = NELEMENTS (mw_event_trans_s1),
    .transitions     = mw_event_trans_s1},
   {/* DU_Event_handling_3 */
    .number_of_trans = NELEMENTS (mw_event_trans_s2),
    .transitions     = mw_event_trans_s2},
};

/**
 * Trigger AL Event FSM state transition
 *
 * This function triggers an AL Event state transition according to the event.
 *
 * @param port          Port handle
 * @param event         AL Event event
 */
static void iolink_mw_event_event (
   iolink_port_t * port,
   iolink_fsm_mw_event_event_t event)
{
   do
   {
      int i;
      iolink_mw_port_t * al            = iolink_get_mw_ctx (port);
      iolink_mw_event_state_t previous = al->event_state;
      const iolink_fsm_mw_event_transition_t * next_trans = NULL;

      for (i = 0; i < iolink_mw_event_fsm[previous].number_of_trans; i++)
      {
         if (iolink_mw_event_fsm[previous].transitions[i].event == event)
         {
            if (!next_trans)
            {
               next_trans = &iolink_mw_event_fsm[previous].transitions[i];
               break;
            }
         }
      }
      if (!next_trans)
      {
         IOL_LOG_ERR (
            "AL Event (%u): next_trans == NULL: state %s - event %s\n",
            iolink_get_portnumber (port),
            iolink_mw_event_state_literals[previous],
            iolink_mw_event_event_literals[event]);
         return;
      }

      /* Transition to next state */
      al->event_state = next_trans->next_state;

      IOL_LOG_DBG (
         "AL Event (%u): event: %s, state transition: %s -> %s\n",
         iolink_get_portnumber (port),
         iolink_mw_event_event_literals[event],
         iolink_mw_event_state_literals[previous],
         iolink_mw_event_state_literals[al->event_state]);

      event = next_trans->action (port, event);
   } while (event != MW_EVENT_EVENT_NONE);
}

/* AL job callback functions */
static void mw_read_write_cb (iolink_job_t * job)
{
   iolink_port_t * port  = job->port;
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   al->service.job_req = job;
   iolink_mw_od_event (port, MW_OD_EVENT_service);
}

static void mw_dl_event_ind_cb (iolink_job_t * job)
{
   iolink_fsm_mw_event_event_t res_event;
   iolink_port_t * port  = job->port;
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);
   diag_entry_t * evt    = &(al->event.events[al->event.event_cnt]);

   // Collect the complete set (1 to 6) of DL_Events of the current EventTrigger
   CC_ASSERT (al->event.event_cnt < 6);
   memcpy (evt, &job->dl_event_ind.event, sizeof (diag_entry_t));
   al->event.event_cnt++;

   if (job->dl_event_ind.eventsleft)
   {
      res_event = MW_EVENT_EVENT_dl_event_ind_more;
   }
   else
   {
      res_event = MW_EVENT_EVENT_dl_event_ind_done;
   }

   iolink_mw_event_event (port, res_event);
}

static void dl_control_ind_cb (iolink_job_t * job)
{
   MW_Control_ind (job->port, job->dl_control_ind.controlcode);
}

static void mw_dl_readparam_cnf_cb (iolink_job_t * job)
{
   iolink_mw_port_t * al = iolink_get_mw_ctx (job->port);

   al->service.data_read[al->service.data_len++] = job->dl_rw_cnf.val;
   al->service.errinfo                           = job->dl_rw_cnf.stat;

   iolink_mw_od_event (job->port, MW_OD_EVENT_readparam_cnf);
}

static void mw_dl_writeparam_cnf_cb (iolink_job_t * job)
{
   bool octets_left               = false;
   iolink_fsm_mw_od_event_t event = MW_OD_EVENT_writeparam_cnf;
   iolink_mw_port_t * al          = iolink_get_mw_ctx (job->port);

   // TODO check job->dl_rw_cnf.stat;

   al->service.errinfo = job->dl_rw_cnf.stat;

   if ((al->service.errinfo == IOLINK_STATUS_NO_ERROR) && octets_left) // TODO
                                                                       // increase
                                                                       // write
                                                                       // pointer
   {
      event = MW_OD_EVENT_rwparam;
   }

   iolink_mw_od_event (job->port, event);
}

static void mw_dl_isdu_transport_cnf_cb (iolink_job_t * job)
{
   iolink_port_t * port  = job->port;
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   al->service.errinfo   = job->dl_rw_cnf.stat;
   al->service.qualifier = job->dl_rw_cnf.qualifier;
   iolink_mw_od_event (port, MW_OD_EVENT_isdutransport_cnf);
}

static void mw_abort_cb (iolink_job_t * job)
{
   iolink_fsm_mw_od_event_t event = MW_OD_EVENT_abort;
   iolink_port_t * port           = job->port;
   iolink_mw_port_t * al          = iolink_get_mw_ctx (port);

   al->service.errinfo  = IOLINK_STATUS_ISDU_ABORT; // TODO what to use here?
   al->service.data_len = 0;

   if (al->od_state == MW_OD_STATE_OnReq_Idle)
   {
      event = MW_OD_EVENT_abort_port;
   }

   iolink_mw_od_event (port, event);
}

static void mw_event_rsp_cb (iolink_job_t * job)
{
   iolink_mw_event_event (job->port, MW_EVENT_EVENT_mw_event_rsp);
}

static void mw_event_req_cb (iolink_job_t * job)
{
   iolink_mw_event_event (job->port, MW_EVENT_EVENT_mw_event_req);
}


void iolink_mw_init (iolink_port_t * port)
{
   iolink_mw_port_t * mw = iolink_get_mw_ctx (port);

   memset (mw, 0, sizeof (iolink_mw_port_t));
   mw->mtx_pdin          = os_mutex_create();

   mw->od_state          = MW_OD_STATE_OnReq_Idle;
   mw->event_state       = MW_EVENT_STATE_Event_idle;
   mw->mw_state          = MW_STATE_Inactive;

   mw->pd_extract_offset = 0;
   mw->pd_extract_len    = 0;
   mw->pd_filtered_len   = 0;
}

iolink_error_t MW_Read_req (
   iolink_port_t * port,
   uint16_t index,
   uint8_t subindex,
   void (*mw_read_cnf_cb) (
      iolink_port_t * port,
      uint8_t len,
      const uint8_t * data,
      iolink_error_t errortype))
{
   iolink_job_t * job        = iolink_fetch_avail_job (port);
   job->mw_read_req.index    = index;
   job->mw_read_req.subindex = subindex;
   CC_ASSERT (mw_read_cnf_cb != NULL);
   job->mw_read_req.mw_read_cb = mw_read_cnf_cb;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_MW_READ_REQ,
      mw_read_write_cb);

   return IOLINK_ERROR_NONE;
}

/* Requirement: Changes to data is not allowed, until after mw_Write_cnf_cb */
iolink_error_t MW_Write_req (
   iolink_port_t * port,
   uint16_t index,
   uint8_t subindex,
   uint8_t len,
   const uint8_t * data,
   void (*mw_write_cnf_cb) (iolink_port_t * port, iolink_error_t errortype))
{
   iolink_job_t * job         = iolink_fetch_avail_job (port);
   job->mw_write_req.index    = index;
   job->mw_write_req.subindex = subindex;
   job->mw_write_req.length   = len;
   job->mw_write_req.data     = data;
   CC_ASSERT (mw_write_cnf_cb != NULL);
   job->mw_write_req.mw_write_cb = mw_write_cnf_cb;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_MW_WRITE_REQ,
      mw_read_write_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t MW_SetOutput_req (iolink_port_t * port, uint8_t * data)
{
   return DL_PDOutputUpdate_req (port, data);
}

iolink_error_t MW_Control_req (iolink_port_t * port, iolink_controlcode_t controlcode)
{
   return DL_Control_req (port, controlcode);
}

iolink_error_t MW_GetInput_req (iolink_port_t * port, uint8_t * len, uint8_t * data)
{
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   os_mutex_lock (al->mtx_pdin);
   *len = al->pdin_data_len;
   memcpy (data, al->pdin_data, *len);
   os_mutex_unlock (al->mtx_pdin);

   return IOLINK_ERROR_NONE;
}

iolink_error_t MW_GetInputOutput_req (
   iolink_port_t * port,
   uint8_t * len,
   uint8_t * data)
{
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   os_mutex_lock (al->mtx_pdin);
   /* PDIn data length */
   uint8_t pdin_len = al->pdin_data_len;
   data[0]          = pdin_len;
   /* PDIn_data[0] */
   uint8_t * pdin_data = &data[1];
   memcpy (pdin_data, al->pdin_data, pdin_len);
   /* PDOut data length */
   uint8_t * pdout_len = &data[pdin_len + 1];
   /* PDOut_data[0] */
   uint8_t * pdout_data = &data[pdin_len + 2];

   iolink_error_t error = DL_PDOutputGet_req (port, pdout_len, pdout_data);

   if (error == IOLINK_ERROR_NONE)
   {
      *len = 1 + pdin_len + 1 + *pdout_len;
   }
   os_mutex_unlock (al->mtx_pdin);

   return error;
}

void DL_Event_ind (
   iolink_port_t * port,
   uint16_t eventcode,
   uint8_t event_qualifier,
   uint8_t eventsleft)
{
   iolink_job_t * job                      = iolink_fetch_avail_job (port);
   job->dl_event_ind.eventsleft            = eventsleft;
   job->dl_event_ind.event.event_code      = eventcode;
   job->dl_event_ind.event.event_qualifier = event_qualifier;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_EVENT_IND,
      mw_dl_event_ind_cb);
}

void DL_Control_ind (iolink_port_t * port, iolink_controlcode_t controlcode)
{
   // TODO check why this is called frequently
   iolink_job_t * job              = iolink_fetch_avail_job (port);
   job->dl_control_ind.controlcode = controlcode;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_CONTROL_IND,
      dl_control_ind_cb);
}

void DL_ReadParam_cnf (iolink_port_t * port, uint8_t value, iolink_status_t errinfo)
{
   iolink_job_t * job  = iolink_fetch_avail_job (port);
   job->dl_rw_cnf.val  = value;
   job->dl_rw_cnf.stat = errinfo;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_READPARAM_CNF,
      mw_dl_readparam_cnf_cb);
}

void DL_WriteParam_cnf (iolink_port_t * port, iolink_status_t errinfo)
{
   iolink_job_t * job  = iolink_fetch_avail_job (port);
   job->dl_rw_cnf.stat = errinfo;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_WRITEPARAM_CNF,
      mw_dl_writeparam_cnf_cb);
}

void DL_ISDUTransport_cnf (
   iolink_port_t * port,
   uint8_t * data,
   uint8_t length,
   iservice_t qualifier,
   iolink_status_t errinfo)
{
   iolink_job_t * job       = iolink_fetch_avail_job (port);
   job->dl_rw_cnf.stat      = errinfo;
   job->dl_rw_cnf.qualifier = qualifier;
   iolink_mw_port_t * al    = iolink_get_mw_ctx (port);

   if (al->service.direction == IOLINK_RWDIRECTION_READ)
   {
      if (errinfo == IOLINK_STATUS_NO_ERROR)
      {
         if ((data != NULL) && (length <= sizeof (al->service.data_read)))
         {
            // TODO can't we pass data_read pointer when we make the initial
            // ISDUTransport_req call?
            memcpy (al->service.data_read, data, length);
            al->service.data_len = length;
         }
         else
         {
            IOL_LOG_ERR (
               "%u: AL: %s, data = %p, length (%u) > %u\n",
               iolink_get_portnumber (port),
               __func__,
               data,
               length,
               (unsigned int)sizeof (al->service.data_read));
         }
      }
   }
   else if (
      (al->service.direction == IOLINK_RWDIRECTION_WRITE) &&
      (qualifier == IOL_ISERVICE_DEVICE_WRITE_RESPONSE_NEG))
   {
      if (data && length <= sizeof (al->service.data_read))
      {
         // TODO can't we pass data_read pointer when we make the initial
         // ISDUTransport_req call?
         memcpy (al->service.data_read, data, length);
         al->service.data_len = length;
      }
      else
      {
         IOL_LOG_ERR (
            "%u: AL: %s, data = %p, length (%u) > %u\n",
            iolink_get_portnumber (port),
            __func__,
            data,
            length,
            (unsigned int)sizeof (al->service.data_read));
      }
   }

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_ISDU_TRANS_CNF,
      mw_dl_isdu_transport_cnf_cb);
}

void DL_PDInputTransport_ind (iolink_port_t * port, uint8_t * pdin_data, uint8_t length)
{
   iolink_mw_port_t * al = iolink_get_mw_ctx (port);

   CC_ASSERT (length <= IOLINK_PD_MAX_SIZE);

   os_mutex_lock (al->mtx_pdin);
   memcpy (al->pdin_data, pdin_data, length);
   al->pdin_data_len = length;
   os_mutex_unlock (al->mtx_pdin);
   MW_NewInput_ind (port);
}


/* Optional application hook: Control indications (e.g. device control codes).
 * Provide a weak default so projects that don't use it still link. */
__attribute__ ((weak)) void MW_Control_ind (
   iolink_port_t * port,
   iolink_controlcode_t controlcode)
{
   (void)port;
   (void)controlcode;
}

/* Public API? */
iolink_error_t MW_Abort (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_api_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_MW_ABORT,
      mw_abort_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t MW_Event_rsp (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_MW_EVENT_RSP,
      mw_event_rsp_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t MW_Event_req (iolink_port_t * port )
{
   iolink_job_t * job = iolink_fetch_avail_api_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_MW_EVENT_REQ,
      mw_event_req_cb);

   return IOLINK_ERROR_NONE;
}

void MW_NewInput_ind(iolink_port_t * port)
{
    iolink_mw_port_t * mw = iolink_get_mw_ctx(port);

    if (mw->mw_state == MW_STATE_Operate)
    {
        iolink_mw_event(port, MW_EVENT_NewInput_Ind);
    }
}

void SM_PortMode_ind (iolink_port_t * port, iolink_sm_portmode_t mode) {
    switch (mode) {
    case IOLINK_SM_PORTMODE_COMREADY:
        iolink_mw_event(port, MW_EVENT_SM_Port_COMREADY);
        break;
    case IOLINK_SM_PORTMODE_OPERATE:
        iolink_mw_event(port, MW_EVENT_SM_Port_OPERATE);
        break;
    case IOLINK_SM_PORTMODE_INACTIVE:
        iolink_mw_event(port, MW_EVENT_Master_Port_Inactive);
        break;
    case IOLINK_SM_PORTMODE_CYCTIME_FAULT:
    	break;
    case IOLINK_SM_PORTMODE_COMLOST:
        iolink_mw_event(port, MW_EVENT_Master_Port_Inactive);
        break;
    default: /* COMP_FAULT, REVISION_FAULT, SERNUM_FAULT */
    }
}

/**
 * @brief Configure the PD filter for a port.
 *
 * Called by Master_ConfigPDFilter() (application-level wrapper) or directly
 * by device-specific setup code (e.g. app_p1py111_setup).
 *
 *   extract_len == 0  → filter disabled; all raw PDIn bytes reach the app.
 *   extract_len >  0  → only extract_len bytes at extract_offset are sent.
 *
 * Thread-safe: offset/len fields are uint8_t (atomic read on all supported
 * architectures); no mutex needed for this config write.
 */
void MW_ConfigPDFilter (
   iolink_port_t * port,
   uint8_t extract_offset,
   uint8_t extract_len)
{
   iolink_mw_port_t * mw = iolink_get_mw_ctx (port);

   mw->pd_extract_offset = extract_offset;
   mw->pd_extract_len    = extract_len;

}


/* Public functions ----------------------------------------------------------*/

/**
 * @brief MW device event hook - called by the MW Event FSM after collecting
 *        all DL events for this EventTrigger cycle.
 */
void MW_Event_ind (
   iolink_port_t * port,
   uint8_t event_cnt,
   diag_entry_t events[6])
{
   uint8_t portnumber  = iolink_get_portnumber (port);
   // uint8_t port_index  = portnumber - 1;

   IOL_LOG_DBG ("MW_Event_ind: port %u, event_cnt=%u\n", portnumber, event_cnt);

   MW_Event_rsp (port);

}


/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/
