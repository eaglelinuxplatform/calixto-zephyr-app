/********************************************************************************
  * @file    iol_cce4511.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    27-Feb-2026
  * @brief   This file contains all the functions prototypes for the DATA LINK LAYER OF IO-LINK.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IOL_CCE4511_H_INCLUDED
#define IOL_CCE4511_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/



 /**
  * IO-Link cce4511 driver configuration
  */
 typedef struct iolink_cce4511_cfg
 {
	 // for future to include the register configuration from the user
	   /** IRQ line used to react to events */
	 uint32_t chip_irq;

	 /** Identification of the SPI slave */
	 const char * spi_slave_name;

	 /** Optional function to read chip registers from the application */
	 void (*register_read_reg_fn) (void * read_reg_function);

 } iolink_cce4511_cfg_t;


 /**
  * Initialises a iolink_cce4511 driver instance and registers
  * the instance.
  *
  * May only be called once per hardware block.
  *
  * @param cfg     Driver configuration.
  * @return        Driver handle or NULL on failure.
  */

  iolink_hw_drv_t * iolink_cce4511_init (const iolink_cce4511_cfg_t *cfg);


#ifdef __cplusplus
}
#endif


/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/


#endif // IOL_CCE4511_H_INCLUDED
