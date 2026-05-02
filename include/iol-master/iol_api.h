/********************************************************************************
  * @file    iol_api.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    02-Apr-2026
  * @brief   This file contains all the functions for the USER OF IO-LINK.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IOL_API_H_INCLUDED
#define IOL_API_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "iol.h"
#include "iol_types.h"
#include "iol_option.h"

 /**
  * @brief Port operating mode
  */
 typedef enum {
 	 PORT_INACTIVE,	 /* Port is disabled */
 	 PORT_ACTIVE,    /* Port is active (IO-Link mode) */
 } PortMode;

 typedef enum{
	 OneTimeSwitchOff,
	 SwitchPowerOff,
	 SwitchPowerOn,
 }PortPowerMode;

 /**
  * Initialise IO-Link master
  *
  * This function initialises the SM state-machine and should be
  * called when the stack is started.
  *
  * @param m_cfg           Master stack configuration
  * @return                Pointer to master information struct
  */
 iolink_m_t * Master_init(const iolink_m_cfg_t * m_cfg);

 /**
  * De-initialise IO-Link master
  *
  * @param m          Master information struct
  */
 void Master_deinit (iolink_m_t ** m);

 /**
  * @brief Set port operating mode
  *
  * This function enables or disables a specific port.
  *
  * @param portnumber   1-based port number
  * @param mode         Desired port mode
  *
  * @return IOLINK_ERROR_NONE on success others on failure
  */

 iolink_error_t Master_setPortMode(
 		uint8_t portnumber,
 		PortMode mode);


 /**
  * @brief Read a device parameter (ISDU)
  *
  * This function reads an indexed parameter from the IO-Link device.
  * It blocks until the read operation completes or times out.
  *
  * @param portnumber   1-based port number
  * @param index        Parameter index
  * @param subindex     Parameter subindex
  * @param buf          Output buffer for received data
  * @param len          Input: buffer size
  *                     Output: number of bytes read
  *
  * @return IOLINK_ERROR_NONE on success
  *         IOLINK_ERROR_MASTER_INIT if master not initialized
  *         IOLINK_ERROR_PARAMETER_CONFLICT for invalid arguments
  *         IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE if port not in OPERATE state
  *         IOLINK_ERROR_STATE_INVALID on timeout or communication failure
  */

 iolink_error_t Master_readParameter(
		 uint8_t portnumber,
		 uint16_t index,
		 uint8_t subindex,
		 uint8_t *buf,
		 uint8_t *len);

 /**
  * @brief Write a device parameter (ISDU)
  *
  * This function writes an indexed parameter to the IO-Link device.
  * It blocks until the write completes or times out.
  *
  * @param portnumber   1-based port number
  * @param index        Parameter index
  * @param subindex     Parameter subindex
  * @param buf          Data to write
  * @param len          Number of bytes to write
  *
  * @return IOLINK_ERROR_NONE                  – Write successful
  *         IOLINK_ERROR_MASTER_INIT           – Master not initialized
  *         IOLINK_ERROR_PARAMETER_CONFLICT    – Invalid parameters
  *         IOLINK_ERROR_SERVICE_TEMP_UNAVAILABLE – Port not in OPERATE state
  *         IOLINK_ERROR_STATE_INVALID         – Timeout or comm error
  */

 iolink_error_t Master_writeParameter(
		 uint8_t portnumber,
		 uint16_t index,
		 uint8_t subindex,
		 const uint8_t *buf,
		 uint8_t len);

 /**
  * @brief Read the latest Process Data Input (PDIn) from the middleware.
  *
  * This is a non-blocking snapshot read. The MW stores a fresh copy of PDIn
  * every time DL_PDInputTransport_ind() is called (every IO-Link cycle), so
  * the data is always ready — no event or semaphore wait is required.
  *
  * @param portnumber   1-based port number
  * @param pd_valid     Output: set to true if the DL pd_valid flag is set (0 - valid, 1 - invalid)
  *                     Pass NULL if you don't need the validity flag.
  * @param data         Caller-allocated buffer to receive PD bytes
  * @param len          buffer size, on exit bytes written
  *
  * @return IOLINK_ERROR_NONE            – data copied successfully
  *         IOLINK_ERROR_MASTER_INIT     – Master_init not called yet
  *         IOLINK_ERROR_PARAMETER_CONFLICT – invalid port or NULL pointers
  *         IOLINK_ERROR_STATE_CONFLICT  – port MW FSM not in OPERATE state
  *         IOLINK_ERROR_PDINLENGTH      – caller buffer too small
  */

 iolink_error_t Master_PDin (
    uint8_t   portnumber,
    bool    * pd_valid,
    uint8_t * data,
    uint8_t * len);

 /**
  * @brief Write Process Data Output (PDOut) to the device.
  * @param portnumber   1-based port number
  * @param pd_valid     0 = output data is valid and ready for the device;
  *                     1 = mark output invalid (device enters safe state)
  * @param data         Output data bytes (must match device's PDOut size).
  *                     Pass NULL with pd_valid=false to just signal invalid.
  * @param len          Number of bytes in data
  *
  * @return IOLINK_ERROR_NONE            – buffer updated successfully
  *         IOLINK_ERROR_MASTER_INIT     – Master_init not called yet
  *         IOLINK_ERROR_PARAMETER_CONFLICT – invalid port or NULL data with len>0
  *         IOLINK_ERROR_STATE_CONFLICT  – port MW FSM not in OPERATE state
  *         IOLINK_ERROR_PDOUTLENGTH     – len exceeds IOLINK_PD_MAX_SIZE
  */

 iolink_error_t Master_PDout (
    uint8_t         portnumber,
    bool            pd_valid,
    const uint8_t * data,
    uint8_t         len);

 /**
  * @brief Configure the PD filter applied inside the MW Process_PD state.
  * @param portnumber      port number
  * @param extract_offset  Byte offset into raw PDIn
  * @param extract_len     Number of bytes to forward (0 = all)
  *
  * @return IOLINK_ERROR_NONE on success
  *         IOLINK_ERROR_MASTER_INIT if Master_init() was not called
  *         IOLINK_ERROR_PARAMETER_CONFLICT if portnumber or len is invalid
  */

 iolink_error_t Master_ConfigPDFilter (
    uint8_t portnumber,
    uint8_t extract_offset,
    uint8_t extract_len);


 //yet to implement - ignore this function for now.


 /**
  * @brief Control port power state
  *
  * This function allows controlling the power supply of a port.
  * It supports turning OFF, turning ON, or cycling power.
  *
  * @param portnumber    1-based port number
  * @param powerMode     Power control mode
  * @param PowerOffTime  Duration (ms) for power-off in OneTimeSwitchOff mode
  *                      Valid range: 500 ms to 65535 ms
  *
  * @return IOLINK_ERROR_NONE on success
  *         IOLINK_ERROR_MASTER_INIT if master not initialized
  *         IOLINK_ERROR_PARAMETER_CONFLICT if parameters invalid
  *         IOLINK_ERROR_STATE_CONFLICT if port not in valid state
  */

 iolink_error_t Master_PortPowerOffOn(
		 uint8_t portnumber,
		 PortPowerMode powerMode,
		 uint16_t PowerOffTime);

#ifdef __cplusplus
}
#endif

#endif // IOL_API_H_INCLUDED

 /************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/
