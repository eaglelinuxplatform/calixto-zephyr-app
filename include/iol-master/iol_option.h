/********************************************************************************
  * @file    iol_sm.h
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
#ifndef IOL_OPTION_H_INCLUDED
#define IOL_OPTION_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Defs ----------------------------------------------------------------------*/

#ifndef IOLINK_MAX_EVENTS
#define IOLINK_MAX_EVENTS (8)
#endif

#ifndef IOLINK_NUM_PORTS
#define IOLINK_NUM_PORTS      (4)
#endif

 /*
  * IO-Link HW
  */
#define IOL_HW_CCE4511 1
#define IOLINK_HW_MAX14819 2
#define IOLINK_HW IOL_HW_CCE4511

#define USER_VENDOR_ID	(0x0057)
#define USER_DEVICE_ID	(0x370F6F)
#define USER_REV_ID     (0x11)
#define USER_FUNC_ID    (0x0)

#ifdef __cplusplus
}
#endif


/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/


#endif // IOL_OPTION_H_INCLUDED
