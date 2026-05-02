/********************************************************************************
  * @file    iol_log.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    27-Feb-2026
  * @brief   This file contains all the functions prototypes for the DATA LINK LAYER OF IO-LINK.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

#ifndef IOL_LOG_H_INCLUDED
#define IOL_LOG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(iol_master);

#  define IOL_LOG_ERR(...)  LOG_ERR(__VA_ARGS__)

#  define IOL_LOG_WRN(...)  LOG_WRN(__VA_ARGS__)

#  define IOL_LOG_INF(...)  LOG_INF(__VA_ARGS__)

#  define IOL_LOG_DBG(...)  LOG_DBG(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* IOL_LOG_H_INCLUDED */