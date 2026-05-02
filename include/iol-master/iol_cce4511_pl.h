/********************************************************************************
  * @file    iol_cce4511_pl.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    25-Feb-2026
  * @brief   This file contains Driver for the CCE4511 IO-Link chip.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IOL_CCE4511_PL_H_INCLUDED
#define IOL_CCE4511_PL_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "iol_pl_hw_drv.h"
#include "iol_cce4511.h"

/* Defs ----------------------------------------------------------------------*/
#define CCE4511_NUM_CHANNELS   4  //for testing 1 is enough
#define CCE4511_CH_MIN 0
#define CCE4511_CH_MAX 3

#define IOL_CCE4511_TX_MAX    (66)
#define REVID_CCE4511 (0x31)
 /* Driver structure */
 typedef struct iol_cce4511_drv
 {
    /* Generic driver interface */
    iolink_hw_drv_t drv;

    void * fd_spi;

    uint32_t pl_flag;

    bool wurq_request[CCE4511_NUM_CHANNELS];
    bool is_iolink[CCE4511_NUM_CHANNELS];

    os_mutex_t *exclusive;

    os_event_t  *dl_event[CCE4511_NUM_CHANNELS];

 } iol_cce4511_drv_t;


 /* Add these declarations */
 extern iol_cce4511_drv_t * g_iolink_drv;
 void iolink_cce4511_isr (void * arg);


#ifdef __cplusplus
}
#endif


/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/


#endif // IOL_CCE4511_PL_H_INCLUDED
