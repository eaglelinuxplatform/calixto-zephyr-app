/********************************************************************************
  * @file    iol_pl.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    24-Feb-2026
  * @brief   This file contains all the functions prototypes for the DATA LINK LAYER OF IO-LINK.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IOL_PL_H_INCLUDED
#define IOL_PL_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "iol-master/iol_dl.h"
#include "iol-master/iol_types.h"
#include <iol-master/iol_pl_hw_drv.h>
#include "iol-master/iol_option.h"
/* Defs ----------------------------------------------------------------------*/



 typedef struct iolink_pl_port
 {
    int fd;
    iolink_hw_drv_t * drv;
    void *            arg;
 } iolink_pl_port_t;


/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/


/* Initialization and Configuration functions --------------------------------*/
 /*
  * Function interface
  */
 void iolink_configure_pl_event (
    iolink_port_t * port,
    os_event_t * event,
    uint32_t flag);
 void iolink_pl_handler (iolink_port_t * port);
 iolink_baudrate_t iolink_pl_get_baudrate (iolink_port_t * port);
 uint8_t iolink_pl_get_cycletime (iolink_port_t * port);
 void iolink_pl_set_cycletime (iolink_port_t * port, uint8_t cycbyte);
 bool iolink_pl_get_data (iolink_port_t * port, uint8_t * rxdata, uint8_t len);
 void iolink_pl_get_error (iolink_port_t * port, uint8_t * cqerr);
 bool iolink_pl_init_sdci (iolink_port_t * port);

 void iolink_pl_port_power_on(iolink_port_t * port);
 void iolink_pl_port_power_off(iolink_port_t * port);

 /*
  * PL Services
  */
 void PL_SetMode_req (iolink_port_t * port, iolink_pl_mode_t mode);
 void PL_WakeUp_req (iolink_port_t * port);
 #if IOLINK_HW == IOLINK_HW_MAX14819
 void PL_Resend (iolink_port_t * port);
 void PL_Transfer_req (
    iolink_port_t * port,
    uint8_t rxbytes,
    uint8_t txbytes,
    uint8_t * data);
 void PL_MessageDownload_req (
    iolink_port_t * port,
    uint8_t rxbytes,
    uint8_t txbytes,
    uint8_t * data);
 void PL_EnableCycleTimer (iolink_port_t * port);
 void PL_DisableCycleTimer (iolink_port_t * port);
#elif IOLINK_HW == IOL_HW_CCE4511
 void PL_Transfer_req (
    iolink_port_t * port,
    uint8_t rxbytes,
    uint8_t txbytes,
    uint8_t * data,
	uint8_t od_txlen,
    uint8_t od_rxlen);
 void PL_MessageDownload_req (
    iolink_port_t * port,
    uint8_t rxbytes,
    uint8_t txbytes,
    uint8_t * data,
	uint8_t od_txlen,
    uint8_t od_rxlen);
 void PL_EnableCycleTimer (iolink_port_t * port);
 void PL_DisableCycleTimer (iolink_port_t * port);
 #endif

 void iolink_pl_init (iolink_port_t * port, iolink_hw_drv_t * drv, void * arg);


#ifdef __cplusplus
}
#endif


/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/


#endif // IOL_PL_H_INCLUDED
