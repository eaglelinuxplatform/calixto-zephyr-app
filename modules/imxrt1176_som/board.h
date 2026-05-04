/*
 * Copyright 2020, 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "IMXRT1176-SOM"
#ifndef DEBUG_CONSOLE_UART_INDEX
#define DEBUG_CONSOLE_UART_INDEX 1
#endif


/*! @brief The board flash size */
#define BOARD_FLASH_SIZE (0x1000000U)

/* SKIP_SEMC_INIT can also be defined independently */
#ifdef USE_SDRAM
#define SKIP_SEMC_INIT
#endif


/* Timer Manager definition. */
#define BOARD_TM_INSTANCE   1
#define BOARD_TM_CLOCK_ROOT kCLOCK_Root_Gpt1

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void BOARD_ConfigMPU(void);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
