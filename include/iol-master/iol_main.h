/********************************************************************************
  * @file    iol_main.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    23-Feb-2026
  * @brief   This file contains all the functions prototypes for the DATA LINK LAYER OF IO-LINK.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IOL_MAIN_H_INCLUDED
#define IOL_MAIN_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <assert.h>
#include "iol_types.h"
#include "iol.h"
#include "iol_log.h"
#include "iol_api.h"
#include "osal.h"

/* Defs ----------------------------------------------------------------------*/
 /**
  * Structure containing all information for one IO-Link port.
  */
 extern iolink_m_t * the_master;

 typedef struct iolink_port iolink_port_t;


 typedef enum iservice
 {
    IOL_ISERVICE_MASTER_NO_SERVICE         = 0b0000,
    IOL_ISERVICE_DEVICE_NO_SERVICE         = 0b0000,
    IOL_ISERVICE_MASTER_WRITE_8I           = 0b0001,
    IOL_ISERVICE_MASTER_WRITE_8I_8SI       = 0b0010,
    IOL_ISERVICE_MASTER_WRITE_16I_16SI     = 0b0011,
    IOL_ISERVICE_DEVICE_WRITE_RESPONSE_NEG = 0b0100,
    IOL_ISERVICE_DEVICE_WRITE_RESPONSE_POS = 0b0101,
    IOL_ISERVICE_MASTER_READ_8I            = 0b1001,
    IOL_ISERVICE_MASTER_READ_8I_8SI        = 0b1010,
    IOL_ISERVICE_MASTER_READ_16I_16SI      = 0b1011,
    IOL_ISERVICE_DEVICE_READ_RESPONSE_NEG  = 0b1100,
    IOL_ISERVICE_DEVICE_READ_RESPONSE_POS  = 0b1101,
    IOL_ISERVICE_MAX                       = 0b1111,
 } iservice_t;


 typedef enum iolink_job_type
 {
    IOLINK_JOB_NONE,
    IOLINK_JOB_PD_EVENT,
	IOLINK_JOB_OPERATE_EVENT,
	IOLINK_JOB_COMLOST_EVENT,
    IOLINK_JOB_SM_OPERATE_REQ,
    IOLINK_JOB_SM_SET_PORT_CFG_REQ,
    IOLINK_JOB_DL_MODE_IND,
    IOLINK_JOB_DL_READ_CNF,
    IOLINK_JOB_DL_WRITE_CNF,
    IOLINK_JOB_DL_WRITE_DEVMODE_CNF,
    IOLINK_JOB_DL_READPARAM_CNF,
    IOLINK_JOB_DL_WRITEPARAM_CNF,
    IOLINK_JOB_DL_ISDU_TRANS_CNF,
    IOLINK_JOB_DL_PDINPUT_TRANS_IND,
    IOLINK_JOB_SM_PORT_MODE_IND,
    IOLINK_JOB_DL_EVENT_IND,
    IOLINK_JOB_DL_CONTROL_IND,
    IOLINK_JOB_DS_STARTUP,
    IOLINK_JOB_DS_DELETE,
    IOLINK_JOB_DS_INIT,
    IOLINK_JOB_DS_UPLOAD,
    IOLINK_JOB_DS_READY,
    IOLINK_JOB_DS_CHANGE,
    IOLINK_JOB_DS_FAULT,
    IOLINK_JOB_OD_START,
    IOLINK_JOB_OD_STOP,
    IOLINK_JOB_MW_CONTROL_CNF,
    IOLINK_JOB_MW_READ_REQ,
    IOLINK_JOB_MW_READ_CNF,
    IOLINK_JOB_MW_WRITE_CNF,
    IOLINK_JOB_MW_WRITE_REQ,
    IOLINK_JOB_MW_ABORT,
    IOLINK_JOB_MW_EVENT_RSP,
    IOLINK_JOB_MW_EVENT_REQ,

    IOLINK_JOB_MW_PORTCONFIGURATION,
    IOLINK_JOB_MW_READBACKPORTCONFIGURATION,
    IOLINK_JOB_MW_PORTSTATUS,
    IOLINK_JOB_MW_DEVICE_WRITE,
    IOLINK_JOB_MW_DEVICE_READ,
    IOLINK_JOB_MW_PARAM_READ,
    IOLINK_JOB_MW_PARAM_WRITE,

    IOLINK_JOB_PERIODIC,

    IOLINK_JOB_EXIT,
 } iolink_job_type_t;

 /** Generic job */
 typedef struct iolink_job
 {
    iolink_job_type_t type;
    iolink_port_t * port;
    void (*callback) (struct iolink_job * job);
    union
    {
       iolink_mhmode_t dl_mode;
       struct
       {
          uint8_t data_len;
          const uint8_t * data;
          uint8_t data_buf[IOLINK_PD_MAX_SIZE];
       } pd_event;
       struct
       {
          iolink_smp_parameterlist_t paramlist;
       } sm_setportcfg_req;
       struct
       {
          uint8_t addr;
          uint8_t val; /* Only used for Read */
          iolink_status_t stat;
          iservice_t qualifier;
       } dl_rw_cnf;
       struct
       {
          const uint8_t * data;
          uint8_t data_len;
          iolink_error_t errortype;
       } mw_read_cnf;
       struct
       {
          iolink_error_t errortype;
       } mw_write_cnf;
       struct
       {
          uint16_t index;
          uint8_t subindex;
          uint8_t val;
          void (*mw_read_cb) (
             iolink_port_t * port,
             uint8_t len,
             const uint8_t * data,
			 iolink_error_t errortype);
       } mw_read_req;
       struct
       {
          uint16_t index;
          uint8_t subindex;
          uint8_t length;
          const uint8_t * data;
          void (*mw_write_cb) (
             iolink_port_t * port,
			 iolink_error_t errortype);
       } mw_write_req;
       struct
       {
          iolink_controlcode_t controlcode;
       } dl_control_ind;
       struct
       {
          iolink_status_t errorinfo;
          iolink_dl_mode_t mode;
       } dl_write_devicemode_cnf;
       struct
       {
          diag_entry_t event;
          uint8_t eventsleft;
       } dl_event_ind;
       struct
       {
          iolink_ds_fault_t fault;
       } ds_fault;
       struct
       {
          iolink_sm_portmode_t mode;
       } sm_port_mode_ind;

    };
 } iolink_job_t;

 typedef struct
 {
    iolink_mhmode_t mh_mode;
    iolink_mhmode_t comrate;
 } iolink_sm_temp_t;

 typedef struct iolink_mw_port iolink_mw_port_t;
 typedef struct iolink_ds_port iolink_ds_port_t;
 typedef struct iolink_pl_port iolink_pl_port_t;
 typedef struct iolink_sm_port iolink_sm_port_t;
 typedef struct iolink_dl iolink_dl_t;

 typedef struct iolink_port_info
 {
    uint32_t deviceid;
    uint16_t vendorid;
    uint16_t functionid;
    uint8_t revisionid;
    uint8_t cycletime;
    iolink_port_quality_info_t port_quality_info;
    iolink_port_status_info_t port_status_info;
    iolink_transmission_rate_t transmission_rate;
    uint8_t serialnumber[16];
 } iolink_port_info_t;

 iolink_job_t * iolink_fetch_avail_job (iolink_port_t * port);
 iolink_job_t * iolink_fetch_avail_api_job (iolink_port_t * port);

 void iolink_post_job_with_type_and_callback (
    iolink_port_t * port,
    iolink_job_t * job,
    iolink_job_type_t type,
    void (*callback) (struct iolink_job * job));

 bool iolink_post_job_pd_event (
    iolink_port_t * port,
    uint32_t timeout,
    uint8_t data_len,
    const uint8_t * data);

 void iolink_post_job_operate_event(
		 iolink_port_t * port);


 /**
  * Get port information
  *
  * This function returns the port information struct for the given port.
  *
  * @param master           Master information struct
  * @param portnumber       Number of the port
  * @return                 Pointer to the port information struct
  */
 iolink_port_t * iolink_get_port (iolink_m_t * master, uint8_t portnumber);

 uint8_t iolink_get_portnumber (iolink_port_t * port);

 uint8_t iolink_get_port_cnt (iolink_port_t * port);

 iolink_port_info_t * iolink_get_port_info (iolink_port_t * port);

 const iolink_smp_parameterlist_t * iolink_get_paramlist (iolink_port_t * port);

 iolink_transmission_rate_t iolink_get_transmission_rate (iolink_port_t * port);

 iolink_mw_port_t * iolink_get_mw_ctx (iolink_port_t * port);

 /**
  * Get the data link layer context
  *
  * @param port           Port information
  * @return               Pointer to data link layer struct
  */
 iolink_dl_t * iolink_get_dl_ctx (iolink_port_t * port);

 iolink_ds_port_t * iolink_get_ds_ctx (iolink_port_t * port);

 iolink_pl_port_t * iolink_get_pl_ctx (iolink_port_t * port);
 iolink_sm_port_t * iolink_get_sm_ctx (iolink_port_t * port);


#ifdef __cplusplus
}
#endif


/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/


#endif // IOL_MAIN_H_INCLUDED
