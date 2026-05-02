/********************************************************************************
  * @file    osal_spi.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    26-Feb-2026
  * @brief   This file contains all the functions prototypes for the DATA LINK LAYER OF IO-LINK.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OSAL_PL_HW_IRQ_H_INCLUDED
#define OSAL_PL_HW_IRQ_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void (*isr_func_t) (void *);

/**
 * A function that creates an interrupt handler thread, which will call the
 * isr_func when a falling edge is detected on the GPIO pin.
 *
 * @param gpio_pin  In: The GPIO pin that is connected to the IRQ pin
 * @param isr_func  In: The function that will be called when the interrupt gets
 * triggered
 * @param irq_arg   In: Pointer to the iolink_hw_drv_t structure containing
 * thread parameter
 */
int _iolink_setup_int (int gpio_pin, isr_func_t isr_func, void * irq_arg);

#ifdef __cplusplus
}
#endif

#endif

