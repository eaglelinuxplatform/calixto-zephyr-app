/********************************************************************************
  * @file    iol_mw.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    27-Mar-2026
  * @brief   This file contains all the functions prototypes for the Middleware OF IO-LINK.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IOL_MW_H_INCLUDED
#define IOL_MW_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "iol_main.h"
#include "iol.h"
#include "iol_types.h"
#include "iol_option.h"

/* Defs ----------------------------------------------------------------------*/

 typedef void (*IOL_callback_t)(uint8_t port, IOL_data_ind type, uint8_t len, const uint8_t *data);


 typedef enum iolink_mw_state
 {
    MW_STATE_Inactive = 0,
	MW_STATE_Startup,
	MW_STATE_Await_Operate,
	MW_STATE_Operate,
	MW_STATE_Process_PD,
	MW_STATE_OD_Exchange,
	MW_STATE_Event_Handling,
	MW_STATE_Recovery,
	MW_STATE_Application_Callback,
	MW_STATE_Port_event,

	MW_STATE_Power_Cycle,

	MW_STATE_LAST
 } iolink_mw_state_t;

 /**
  * MW state-machine events.
  *
  * MW events trigger state transitions in FSM.
  */
 typedef enum iolink_fsm_mw_event
 {
     MW_EVENT_NONE = 0,
     MW_EVENT_Master_Port_Active,
     MW_EVENT_Master_Port_Inactive,
     MW_EVENT_SM_Port_COMREADY,
     MW_EVENT_SM_Port_OPERATE,
     MW_EVENT_NewInput_Ind,
     MW_EVENT_isdu_read,
     MW_EVENT_isdu_write,
     MW_EVENT_isdu_complete,
     MW_EVENT_event_handling,
     MW_EVENT_event_complete,
     MW_EVENT_application_cb,
     MW_EVENT_Timeout_no_device,
     MW_EVENT_abort_port,

	 MW_EVENT_power_off_req,
	 MW_EVENT_power_on_req,
	 MW_EVENT_power_one_time_off,
	 MW_EVENT_power_cycle_complete,

	 MW_EVENT_LAST
 } iolink_fsm_mw_event_t;


 typedef enum iolink_mw_od_state
 {
    MW_OD_STATE_OnReq_Idle = 0,
    MW_OD_STATE_Build_DL_Service,
    MW_OD_STATE_Await_DL_param_cnf,
    MW_OD_STATE_Await_DL_ISDU_cnf,
    MW_OD_STATE_LAST
 } iolink_mw_od_state_t;

 typedef enum iolink_mw_event_state
 {
    MW_EVENT_STATE_Event_inactive = 0,
    MW_EVENT_STATE_Event_idle,
    MW_EVENT_STATE_Read_Event_Set,
    MW_EVENT_STATE_DU_Event_handling,
    MW_EVENT_STATE_LAST
 } iolink_mw_event_state_t;


 typedef struct iolink_port_power{
 	PortPowerMode powerMode;
 	uint16_t offSetTime;
 }iolink_port_power_t;

 typedef struct iolink_mw_port
 {
    iolink_mw_od_state_t od_state;
    iolink_mw_event_state_t event_state;
    iolink_mw_state_t mw_state;

    /* Raw PDIn snapshot — written by DL, read by MW Process_PD action */
    uint8_t pdin_data[IOLINK_PD_MAX_SIZE];
    uint8_t pdin_data_len;
    os_mutex_t  *mtx_pdin;

    uint8_t pd_extract_offset;
    uint8_t pd_extract_len;

    uint8_t pd_filtered[IOLINK_PD_MAX_SIZE];
    uint8_t pd_filtered_len;

    struct
    {
       uint16_t index;
       uint8_t subindex;
       iolink_rwdirection_t direction;
       uint8_t data_read[IOLINK_ISDU_MAX_SIZE];
       const uint8_t * data_write;
       uint8_t data_len;
       iolink_status_t errinfo;
       iservice_t qualifier;
       iolink_job_t * job_req;
       union
       {
          void (*mw_read_cnf_cb) (
             iolink_port_t * port,
             uint8_t len,
             const uint8_t * data,
			 iolink_error_t errortype);
          void (*mw_write_cnf_cb) (
             iolink_port_t * port,
			 iolink_error_t errortype);
       };
    } service;

    struct
    {
       uint8_t event_cnt;
       diag_entry_t events[IOLINK_MAX_EVENTS];
    } event;

    iolink_port_power_t port_pwr_info;

 } iolink_mw_port_t;

 /**
  * MW OD state-machine events.
  *
  * MW events trigger state transitions in the OD FSM.
  */
 typedef enum iolink_fsm_mw_od_event
 {
    MW_OD_EVENT_NONE = 0,
    MW_OD_EVENT_service,           /* T1, T16, T8 and T12 */
    MW_OD_EVENT_arg_err,           /* T2 */
    MW_OD_EVENT_param_read,        /* T3 */
    MW_OD_EVENT_param_write_1,     /* T4 */
    MW_OD_EVENT_param_write_2,     /* T5 */
    MW_OD_EVENT_isdu_read,         /* T6 */
    MW_OD_EVENT_isdu_write,        /* T7 */
    MW_OD_EVENT_abort,             /* T9 and T11 */
    MW_OD_EVENT_rwparam,           /* T10 */
    MW_OD_EVENT_readparam_cnf,     /* T13 */
    MW_OD_EVENT_writeparam_cnf,    /* T14 */
    MW_OD_EVENT_isdutransport_cnf, /* T15 */
    MW_OD_EVENT_abort_port,        /* T17 */
    MW_OD_EVENT_LAST
 } iolink_fsm_mw_od_event_t;

 /**
  * MW Event state-machine events.
  *
  * MW events trigger state transitions in the Event FSM.
  */
 typedef enum iolink_fsm_mw_event_event
 {
    MW_EVENT_EVENT_NONE = 0,
    MW_EVENT_EVENT_dl_event_ind_more, /* T3 or T4 */
    MW_EVENT_EVENT_dl_event_ind_done, /* T5 or T3 + T5 */
    MW_EVENT_EVENT_mw_event_rsp,      /* T6 */
    MW_EVENT_EVENT_mw_event_req,      /* T7 */
    MW_EVENT_EVENT_LAST
 } iolink_fsm_mw_event_event_t;


 iolink_error_t MW_Read_req (
    iolink_port_t * port,
    uint16_t index,
    uint8_t subindex,
    void (*mw_read_cnf_cb) (
       iolink_port_t * port,
       uint8_t len,
       const uint8_t * data,
       iolink_error_t errortype));

 iolink_error_t MW_Write_req (
    iolink_port_t * port,
    uint16_t index,
    uint8_t subindex,
    uint8_t len,
    const uint8_t * data,
    void (*mw_write_cnf_cb) (
       iolink_port_t * port,
	   iolink_error_t errortype));

 iolink_error_t MW_SetOutput_req (
		 iolink_port_t * port,
		 uint8_t * data);

 iolink_error_t MW_Control_req (
    iolink_port_t * port,
    iolink_controlcode_t controlcode);

 iolink_error_t MW_GetInput_req (
		 iolink_port_t * port,
		 uint8_t * len,
		 uint8_t * data);

 iolink_error_t MW_GetInputOutput_req (
    iolink_port_t * port,
    uint8_t * len,
    uint8_t * data);

 void MW_NewInput_ind(
		 iolink_port_t * port);

 void DL_Event_ind (
    iolink_port_t * port,
    uint16_t eventcode,
    uint8_t event_qualifier,
    uint8_t eventsleft);

 void DL_Control_ind (
		 iolink_port_t * port,
		 iolink_controlcode_t controlcode);

 void DL_ReadParam_cnf (
		 iolink_port_t * port,
		 uint8_t value,
		 iolink_status_t errinfo);

 void DL_WriteParam_cnf (
		 iolink_port_t * port,
		 iolink_status_t errinfo);

 void DL_ISDUTransport_cnf (
    iolink_port_t * port,
    uint8_t * data,
    uint8_t length,
    iservice_t qualifier,
    iolink_status_t errinfo);

 void DL_PDInputTransport_ind (
    iolink_port_t * port,
    uint8_t * inputdata,
    uint8_t length);

/* Optional application hook: Control indications (e.g. device control codes) */
void MW_Control_ind (
   iolink_port_t * port,
   iolink_controlcode_t controlcode);

 void MW_Event_ind (
    iolink_port_t * port,
    uint8_t event_cnt,
    diag_entry_t events[6]); // TODO where to put this?

 iolink_error_t MW_Abort (
		 iolink_port_t * port);

iolink_error_t MW_Event_rsp (
		 iolink_port_t * port);

/* MW main state machine trigger (used by master/SM glue) */
void iolink_mw_event (
   iolink_port_t * port,
   iolink_fsm_mw_event_t event);

 void iolink_mw_init (
		 iolink_port_t * port);

 /**
  * Configure the PD filter applied in the MW Process_PD state.
  *
  * The middleware extracts extract_len bytes starting at extract_offset from
  * the raw PDIn and sends only those bytes to the application callback.
  * Set extract_len = 0 to disable filtering (pass all raw PDIn bytes).
  *
  * @param port            Port handle
  * @param extract_offset  Byte offset of the measurement in raw PDIn
  * @param extract_len     Number of bytes to forward (0 = pass all)
  */
 void MW_ConfigPDFilter (
    iolink_port_t * port,
    uint8_t extract_offset,
    uint8_t extract_len);

/* Initialization and Configuration functions --------------------------------*/


#ifdef __cplusplus
}
#endif

#endif // IOL_MW_H_INCLUDED

 /************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/
