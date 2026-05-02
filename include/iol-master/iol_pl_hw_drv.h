/********************************************************************************
  * @file    iol_pl_hw_drv.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    25-Feb-2026
  * @brief   This file contains all the functions prototypes for the Physical Layer Hardware driver of io-link.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IOL_PL_HW_DRV_H_INCLUDED
#define IOL_PL_HW_DRV_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "iol.h"
#include "iol_types.h"
#include "osal.h"

 struct iolink_hw_drv;

 typedef struct iolink_hw_ops
 {
    iolink_baudrate_t (*get_baudrate) (struct iolink_hw_drv * iolink_hw, void * arg);
    uint8_t (*get_cycletime) (struct iolink_hw_drv * iolink_hw, void * arg);
    void (*set_cycletime) (
       struct iolink_hw_drv * iolink_hw,
       void * arg,
       uint8_t cycbyte);
    bool (*set_mode) (
       struct iolink_hw_drv * iolink_hw,
       void * arg,
       iolink_pl_mode_t mode);
    void (*wakeup_req)(struct iolink_hw_drv * iolink_hw,void * arg);
    void (*enable_cycle_timer) (struct iolink_hw_drv * iolink_hw, void * arg);
    void (*disable_cycle_timer) (struct iolink_hw_drv * iolink_hw, void * arg);
    void (*get_error) (
       struct iolink_hw_drv * iolink_hw,
       void * arg,
       uint8_t * cqerr);
    bool (*get_data) (
       struct iolink_hw_drv * iolink_hw,
       void * arg,
       uint8_t * rxdata,
       uint8_t len);
    void (*dl_msg) (
       struct iolink_hw_drv * iolink_hw,
       void * arg,
       uint8_t rxbytes,
       uint8_t txbytes,
       uint8_t * data,
		uint8_t od_txlen,
	    uint8_t od_rxlen);
    void (*send_msg) (struct iolink_hw_drv * iolink_hw, void * arg);
    void (*transfer_req) (
       struct iolink_hw_drv * iolink_hw,
       void * arg,
       uint8_t rxbytes,
       uint8_t txbytes,
       uint8_t * data,
		uint8_t od_txlen,
	   uint8_t od_rxlen);
    bool (*init_sdci) (struct iolink_hw_drv * iolink_hw, void * arg);
    void (*configure_event) (
       struct iolink_hw_drv * iolink_hw,
       void * arg,
       os_event_t * event,
       uint32_t flag);
    void (*pl_handler) (struct iolink_hw_drv * iolink_hw, void * arg);
    void (*port_power_off)(struct iolink_hw_drv * iolink_hw, void * arg);
    void (*port_power_on)(struct iolink_hw_drv * iolink_hw, void * arg);

 } iolink_hw_ops_t;

 typedef struct iolink_hw_drv
 {
    os_mutex_t      *      mtx;
    const iolink_hw_ops_t * ops;
 } iolink_hw_drv_t;

 void iolink_hw_init (iolink_hw_drv_t * iolink_hw);

#ifdef __cplusplus
}
#endif


/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/


#endif // IOL_PL_HW_DRV_H_INCLUDED
