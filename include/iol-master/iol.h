/********************************************************************************
  * @file    iol.h
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
#ifndef IOL_H_INCLUDED
#define IOL_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "iol_option.h"

/* Defs ----------------------------------------------------------------------*/
#define IOLINK_REVISION_1_1 0x11
#define IOLINK_PD_MAX_SIZE 32 //!< Maximum number of bytes in Process Data

 typedef struct iolink_m_cfg iolink_m_cfg_t;
 typedef struct iolink_m iolink_m_t;
 typedef uint8_t iolink_port_qualifier_info_t; //!< Port Qualifier Information
                                               //!< (PQI)
 typedef uint8_t iolink_port_quality_info_t;   //!< Port Quality Information

 typedef enum {
 	 PD_DATA_IND,
	 PORT_OPERATE_IND,
	 PORT_COMLOST_IND,
 	 EVENT_DATA_IND,
 } IOL_data_ind;

 /**
  * IO-Link PL modes
  */
 typedef enum
 {
    /** Inactive */
    iolink_mode_INACTIVE = 0,

    /** Digital out */
    iolink_mode_DO       = 1,

    /** Digital in */
    iolink_mode_DI       = 2,

    /** IO-Link signal mode */
	iolink_mode_SDCI     = 3,

 } iolink_pl_mode_t;

 /**
  * IO-Link general error types
  */
 typedef enum
 {
	IOLINK_ERROR_MASTER_INIT = -3,
	IOLINK_ERROR_NO_COMM  = -2,
	IOLINK_ERROR_STATE_CONFLICT = -1 ,
    IOLINK_ERROR_NONE = 0,
    IOLINK_ERROR_PDINLENGTH,
    IOLINK_ERROR_PDOUTLENGTH,
    IOLINK_ERROR_ODLENGTH,
    IOLINK_ERROR_ADDRESS_INVALID,
    IOLINK_ERROR_COMCHANNEL_INVALID,
    IOLINK_ERROR_MODE_INVALID,
    IOLINK_ERROR_STATUS_INVALID,
    IOLINK_ERROR_CONTROLCODE_INVALID,
    IOLINK_ERROR_CONDITIONS_NOT_CORRECT,
    IOLINK_ERROR_INCORRECT_DATA,
    IOLINK_ERROR_STATE_INVALID,
    IOLINK_ERROR_OUT_OF_MEMORY,
    IOLINK_ERROR_PARAMETER_CONFLICT,
	IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE,
	IOLINK_ERROR_ISDU_CHECKSUM_ERROR,
	IOLINK_ERROR_DEV_APPLICATION,

//    IOLINK_ERROR_STATE_CONFLICT,
 } iolink_error_t;



 /**
  * IO-Link EventCodes
  *
  * IO-Link Interface Spec v1.1.5 Table D.2 EventCodes for Ports
  */

 typedef enum  iolink_eventcode
 {
    IOLINK_EVENTCODE_NONE              = 0x0000,
    IOLINK_EVENTCODE_NO_DEV            = 0x1800,
    IOLINK_EVENTCODE_START_PARAM_ERR   = 0x1801,
    IOLINK_EVENTCODE_BAD_VID           = 0x1802,
    IOLINK_EVENTCODE_BAD_DID           = 0x1803,
    IOLINK_EVENTCODE_SHORT_CIR_CQ      = 0x1804,
    IOLINK_EVENTCODE_PHY_OVERTEMP      = 0x1805,
    IOLINK_EVENTCODE_SHORT_CIR_L       = 0x1806,
    IOLINK_EVENTCODE_OVERCUR_L         = 0x1807,
    IOLINK_EVENTCODE_DEV_EVENT_OVERF   = 0x1808,
    IOLINK_EVENTCODE_BACKUP_INCON_SIZE = 0x1809,
    IOLINK_EVENTCODE_BACKUP_INCON_ID   = 0x180A,
    IOLINK_EVENTCODE_BACKUP_INCON      = 0x180B,
    IOLINK_EVENTCODE_BACKUP_INCON_UL   = 0x180C,
    IOLINK_EVENTCODE_BACKUP_INCON_DL   = 0x180D,
    IOLINK_EVENTCODE_P24               = 0x180E,
    IOLINK_EVENTCODE_P24_SHORT_CIR     = 0x180F,
    IOLINK_EVENTCODE_SHORT_CIR_IQ      = 0x1810,
    IOLINK_EVENTCODE_SHORT_CIR_DO_CQ   = 0x1811,
    IOLINK_EVENTCODE_OVERCUR_IQ        = 0x1812,
    IOLINK_EVENTCODE_OVERCUR_CQ        = 0x1813,

    IOLINK_EVENTCODE_INVAL_CYCTIME = 0x6000,
    IOLINK_EVENTCODE_REV_FAULT     = 0x6001,
    IOLINK_EVENTCODE_ISDU_FAIL     = 0x6002,

    IOLINK_EVENTCODE_PORT_STATUS_CHANGE = 0xFF26,
    IOLINK_EVENTCODE_DS_UPLOAD_DONE     = 0xFF27,

    IOLINK_EVENTCODE_DEV_DS_UPLOAD_REQ = 0xFF91,
 } iolink_eventcode_t;

 /**
  * Port modes
  */

 typedef enum iolink_portmode
 {
    IOLINK_PORTMODE_DEACTIVE = 0,
    IOLINK_PORTMODE_IOL_MAN  = 1,
    IOLINK_PORTMODE_IOL_AUTO = 2,
    IOLINK_PORTMODE_DI_CQ    = 3,
    IOLINK_PORTMODE_DO_CQ    = 4,
 } iolink_portmode_t;

 /**
  * Validation check values
  */

 typedef enum  iolink_validation_check
 {
    IOLINK_VALIDATION_CHECK_NO              = 0,
    IOLINK_VALIDATION_CHECK_V10             = 1,
    IOLINK_VALIDATION_CHECK_V11             = 2,
    IOLINK_VALIDATION_CHECK_V11_BAK_RESTORE = 3,
    IOLINK_VALIDATION_CHECK_V11_RESTORE     = 4,
 } iolink_validation_check_t;

 typedef enum  iolink_iq_behavior
 {
    IOLINK_IQ_BEHAVIOR_NO_SUPPORT = 0,
    IOLINK_IQ_BEHAVIOR_DI         = 1,
    IOLINK_IQ_BEHAVIOR_DO         = 2,
    IOLINK_IQ_BEHAVIOR_RES1       = 3,
    IOLINK_IQ_BEHAVIOR_RES2       = 4,
    IOLINK_IQ_BEHAVIOR_POWER2     = 5,
 } iolink_iq_behavior_t;

 typedef enum
 {
    IOLINK_PORT_TYPES_A            = 0,
    IOLINK_PORT_TYPES_A_POWER      = 1,
    IOLINK_PORT_TYPES_B            = 2,
    IOLINK_PORT_TYPES_FS_A_NO_OSSD = 3,
    IOLINK_PORT_TYPES_FS_A_OSSD    = 4,
    IOLINK_PORT_TYPES_FS_B         = 5,
    IOLINK_PORT_TYPES_W_MASTER     = 6,
 } iolink_port_types_t;

 /**
  * Port Status Information
  */

 typedef enum  iolink_port_status_info
 {
    IOLINK_PORT_STATUS_INFO_NO_DEV      = 0,
    IOLINK_PORT_STATUS_INFO_DEACTIVATED = 1,
    IOLINK_PORT_STATUS_INFO_PORT_DIAG   = 2,
    IOLINK_PORT_STATUS_INFO_RES         = 3,
    IOLINK_PORT_STATUS_INFO_PREOP       = 3, //NOT IN V.1.1.5 SPEC
    IOLINK_PORT_STATUS_INFO_OP          = 4,
    IOLINK_PORT_STATUS_INFO_DI          = 5,
    IOLINK_PORT_STATUS_INFO_DO          = 6,
    IOLINK_PORT_STATUS_INFO_POWER_OFF   = 254,
    IOLINK_PORT_STATUS_INFO_NOT_AVAIL   = 255,
 } iolink_port_status_info_t;

 /* Port Quality Info bits, iolink_port_quality_info_t */
 /* PDIn bit0 */
 #define IOLINK_PORT_QUALITY_INFO_BIT     0
 #define IOLINK_PORT_QUALITY_INFO_VALID   0
 #define IOLINK_PORT_QUALITY_INFO_INVALID BIT (IOLINK_PORT_QUALITY_INFO_BIT)
 /* PDOut bit1 */
 #define IOLINK_PORT_QUALITY_INFO_PDO_BIT   1
 #define IOLINK_PORT_QUALITY_INFO_PDO_VALID 0
 #define IOLINK_PORT_QUALITY_INFO_PDO_INVALID                                   \
    BIT (IOLINK_PORT_QUALITY_INFO_PDO_BIT)

 /* Port Qualifier Information bits, iolink_port_qualifier_info_t */
 /* DevCom bit5 */
 #define IOLINK_PORT_QUALIFIER_INFO_COM_BIT   5
 #define IOLINK_PORT_QUALIFIER_INFO_COM_VALID 0
 #define IOLINK_PORT_QUALIFIER_INFO_COM_INVALID                                 \
    BIT (IOLINK_PORT_QUALIFIER_INFO_COM_BIT)
 /* DevErr bit6 */
 #define IOLINK_PORT_QUALIFIER_INFO_ERR_BIT   6
 #define IOLINK_PORT_QUALIFIER_INFO_ERR_VALID 0
 #define IOLINK_PORT_QUALIFIER_INFO_ERR_INVALID                                 \
    BIT (IOLINK_PORT_QUALIFIER_INFO_ERR_BIT)
 /* PQ bit7 */
 #define IOLINK_PORT_QUALIFIER_INFO_PQ_BIT   7
 #define IOLINK_PORT_QUALIFIER_INFO_PQ_VALID 0
 #define IOLINK_PORT_QUALIFIER_INFO_PQ_INVALID                                  \
    BIT (IOLINK_PORT_QUALIFIER_INFO_PQ_BIT) //!< Port Qualifier bit in QPI

 /**
  * Transmission rates
  */

 typedef enum  iolink_transmission_rate
 {
    IOLINK_TRANSMISSION_RATE_NOT_DETECTED = 0,
    IOLINK_TRANSMISSION_RATE_COM1         = 1,
    IOLINK_TRANSMISSION_RATE_COM2         = 2,
    IOLINK_TRANSMISSION_RATE_COM3         = 3,
 } iolink_transmission_rate_t;




 typedef enum  iolink_master_type
 {
    IOLINK_MASTER_TYPE_UNSPEC     = 0,
    IOLINK_MASTER_TYPE_RESERVED   = 1,
    IOLINK_MASTER_TYPE_MASTER_ACC = 2,
    IOLINK_MASTER_TYPE_FS         = 3,
    IOLINK_MASTER_TYPE_W          = 4,
 } iolink_master_type_t;


 typedef struct  portconfiglist
 {
    iolink_portmode_t portmode;
    iolink_validation_check_t validation_backup;
    iolink_iq_behavior_t iq_behavior;
    uint8_t port_cycle_time;
    uint16_t vendorid;
    uint32_t deviceid;
    uint8_t in_buffer_len;
    uint8_t out_buffer_len;
 } portconfiglist_t;



 typedef struct  arg_block_portconfiglist
 {
    portconfiglist_t configlist;
 } arg_block_portconfiglist_t;



 typedef struct  diag_entry
 {
    uint8_t event_qualifier;
    iolink_eventcode_t event_code;
 } diag_entry_t;


 typedef struct  arg_block_portstatuslist
 {
    iolink_port_status_info_t port_status_info;
    iolink_port_quality_info_t port_quality_info;
    uint8_t revision_id;
    iolink_transmission_rate_t transmission_rate;
    uint8_t master_cycle_time;
    uint8_t reserved;
    uint16_t vendorid;
    uint32_t deviceid;
    uint8_t number_of_diags;
    diag_entry_t * diag_entries;
 } arg_block_portstatuslist_t;



 typedef struct  arg_block_devevent
 {
    diag_entry_t diag_entry[6];         // TODO diag_entry_t diag_entry
    uint8_t event_count;                // TODO remove
 } arg_block_devevent_t;



 typedef struct  arg_block_portevent
 {
    diag_entry_t event;
 } arg_block_portevent_t;



 typedef struct iolink_hw_drv iolink_hw_drv_t;

 /** Port configuration */
 typedef struct iolink_port_cfg
 {
//    /** Port name */
//    const char * name;

    /** Port mode */
    iolink_pl_mode_t * mode;

    /** Driver */
    iolink_hw_drv_t * drv;

    /** Cycle Time **/
    uint8_t cyc_time;

    /** User argument */
    void * arg;
 } iolink_port_cfg_t;

 /** IO-Link master stack configuration */
 typedef struct iolink_m_cfg
 {
    /** Callback opaque argument */
    void * cb_arg;

    /**  data callback function */
    void (*user_cb) (
       uint8_t portnumber,
       void * arg,
	   IOL_data_ind data_type,
       uint8_t data_len,
       const uint8_t * data);

    /** Number of connected IO-Link ports */
    uint8_t port_cnt;

    /** Configuration (name and mode) of the connected IO-Link ports */
    iolink_port_cfg_t * port_cfgs;

    /** Priority of the master thread */
    unsigned int master_thread_prio;

    /** Stack size (in bytes) of the master thread */
    size_t master_thread_stack_size;

    /** Priority of the DL thread */
    unsigned int dl_thread_prio;

    /** Stack size (in bytes) of the DL thread */
    size_t dl_thread_stack_size;
 } iolink_m_cfg_t;


/* Initialization and Configuration functions --------------------------------*/


#ifdef __cplusplus
}
#endif


/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/


#endif // IOL_H_INCLUDED
