/**
  ******************************************************************************
  * @file    iol_cce4511_pl.c
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    25-Feb-2026
  * @brief   This file provides functions to manage the following
  *          breif the functionalities here:
  *           - SPI register read/write (single + burst)
  *           - Chip reset and channel configuration
  *           - M-sequence master message setup (Type 0 / 1 / 2)
  *           - Frame Handler interrupt dispatch
  *
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

#include <iol-master/iol_cce4511_pl.h>
#include "iol-master/cce4511_reg_def.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "iol-master/osal.h"
#include "iol-master/iol_log.h"
#include "iol-master/iol_main.h"
#include "iol-master/iol_dl.h"
#include "iol-master/osal_irq.h"
#include "iol-master/osal_spi.h"

/* Private macros -------------------------------------------------------------*/

/* Default values */
#define CCE4511_REVID         0x31

#define IOLINK_MC_MASK       0x60

#define IOLINK_MC_PROS       0x00
#define IOLINK_MC_PAGE       0x20
#define IOLINK_MC_DIAG       0x40
#define IOLINK_MC_ISDU       0x60

#define IOLINK_CKT_MASK        0xC0
#define IOLINK_CKT_TYPE0       0x00
#define IOLINK_CKT_TYPE1       0x40
#define IOLINK_CKT_TYPE2       0x80

#define IOLINK_MC_ADDRESS_MASK    0x1F   /**< On-request data address [3:0] */

#define IOLINK_TYPE1_PD_BYTES         2

/**
 * @brief Decoded M-sequence type enumeration
 */
typedef enum
{
   MSEQ_TYPE_0,         /**< Type 0: single OD byte, no PD */
   MSEQ_TYPE_1,         /**< Type 1: master read, OD only in response */
   MSEQ_TYPE_2,         /**< Type 2: combined PD and OD in both directions */
   MSEQ_TYPE_UNKNOWN    /**< Unrecognised CKT encoding */
} mseq_type_t;

typedef enum
{
   PROCESS_CHN_READ,
   PROCESS_CHN_WRITE,
   PAGE_CHN_READ,
   PAGE_CHN_WRITE,
   DIAG_CHN_READ,
   DIAG_CHN_WRITE,
   ISDU_CHN_READ,
   ISDU_CHN_WRITE,
} c_chn_type_t;

iol_cce4511_drv_t * g_iolink_drv = NULL;

/* Private variables ---------------------------------------------------------*/
const uint8_t reg_vals[CCE4511_NUM_OF_REG_CHN]={
0xF0, /* CFG1 */
0x73, /* CFG2 */
0xfc, /* CFG3 */
0x00, /* SIO  */
0x00, /* UART */
0x20, /* FHC */
0x00, /* OD */
0x00, /* MPD */
0x00, /* DPD */
0x00, /* CYCT */
0x00, /* FHD */
0x00, /* BLVL */
0xCC, /* LSEQ_A */
0x80, /* LHLD_A */
0xF0, /* LSEQ_B */
0x80, /* LHLD_B */
0x18, /* CFG4 */
0x00, /* WRP_SOTO_TIMER */
0x00, /* STATUS_MODE */
0x00, /* INT_SRC_STAT */
0x00, /* INT_EN_STAT */
0x00, /* INT_SRC_SIO */
0x00, /* INT_EN_SIO */
0x00, /* INT_SRC_UART */
0x00, /* INT_EN_UART */
0x00, /* INT_SRC_FH */
0xFF, /* INT_EN_FH */
0x00, //0xaF, /* LDRV_A */
0x00, //0xaF, /* LDRV_B */
0x11, /* LPX_CLA_TIME */
0x11, /* LPX_OVC_TIME */
0x11  /* CQ_OVC_TIME */
};

/* Private functions ---------------------------------------------------------*/


/**
 * @file
 * @brief Driver for the CCE4511 IO-Link chip
 *
 * The iolink_cce4511 driver is used for communication with a CCE4511
 * IO-Link chip. The chip contains four IO-Link master transceivers with logic.
 *
 */


static void iolink_pl_cce4511_pl_handler (iolink_hw_drv_t * iolink_hw, void * arg);
void iolink_cce4511_isr (void * arg);

static cce4511_reg_addr cce4511_get_chn_reg_base_addr(cce4511_channel_t chn)
{
    switch(chn)
    {
        case CCE4511_CHN_0:
            return CCE4511_CHN0_CFG1_REG_ADDR;
        case CCE4511_CHN_1:
            return CCE4511_CHN1_CFG1_REG_ADDR;
        case CCE4511_CHN_2:
            return CCE4511_CHN2_CFG1_REG_ADDR;
        case CCE4511_CHN_3:
            return CCE4511_CHN3_CFG1_REG_ADDR;
        default:
            return 0U;
    }
}

static mseq_type_t decode_mseq_type(uint8_t ckt)
{
   switch (ckt & IOLINK_CKT_MASK)
   {
      case IOLINK_CKT_TYPE0:
         return MSEQ_TYPE_0;

      case IOLINK_CKT_TYPE1:
         return MSEQ_TYPE_1;

      case IOLINK_CKT_TYPE2:
         return MSEQ_TYPE_2;

      default:
         return MSEQ_TYPE_UNKNOWN;
   }
}

static void iolink_cce4511_write_register (
   iol_cce4511_drv_t * iolink,
   uint8_t reg,
   uint8_t value)
{
   uint8_t wbuf[3];
   uint8_t rbuf[3];

   CC_ASSERT (iolink->fd_spi >= 0);

   wbuf[0] = (uint8_t) (reg);
   wbuf[1] = (uint8_t)(1U << 3);
   wbuf[1] &= (uint8_t)(~(1U));
   wbuf[2] = value;

   _iolink_pl_hw_spi_transfer(iolink->fd_spi, &rbuf, &wbuf, sizeof (wbuf));
}

static uint8_t iolink_cce4511_read_register (
		iol_cce4511_drv_t * iolink,
		uint8_t reg)
{
   uint8_t wbuf[3];
   uint8_t rbuf[3];

   CC_ASSERT (iolink->fd_spi >= 0);

   wbuf[0] = (uint8_t) (reg);
   wbuf[1] = (uint8_t)(1U << 3);
   wbuf[1] |= 1U;
   wbuf[2] = 0U;

   _iolink_pl_hw_spi_transfer (iolink->fd_spi, &rbuf, &wbuf, sizeof (wbuf));

   return rbuf[2];
}

static void write_od_mpd_dpd(iol_cce4511_drv_t * iolink, uint8_t base_addr,
                              uint8_t od,
                              uint8_t mpd,
                              uint8_t dpd)
{
   uint8_t reg_addr;

   reg_addr = base_addr + (CCE4511_CHN0_OD_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);
   iolink_cce4511_write_register(iolink, reg_addr, od);

   reg_addr = base_addr + (CCE4511_CHN0_MPD_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);
   iolink_cce4511_write_register(iolink, reg_addr, mpd);

   reg_addr = base_addr + (CCE4511_CHN0_DPD_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);
   iolink_cce4511_write_register(iolink, reg_addr, dpd);
}

static void iolink_cce4511_burst_write_tx (
   iol_cce4511_drv_t * iolink,
   cce4511_channel_t ch,
   uint8_t * data,
   uint8_t size)
{
	cce4511_reg_addr fhd_reg = cce4511_get_chn_reg_base_addr(ch)
		   + (CCE4511_CHN0_FHD_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);
   uint8_t data_tx[IOLINK_RXTX_BUFFER_SIZE + 2];
   uint8_t data_rx[IOLINK_RXTX_BUFFER_SIZE + 2] = {0};

   uint8_t len = (size > 31U) ? 31U : size;

   data_tx[0] = (uint8_t) (fhd_reg);
   data_tx[1] = (uint8_t)(len << 3);
   data_tx[1] &= (uint8_t)(~(1U));

   memcpy (&data_tx[2], data, size);

   _iolink_pl_hw_spi_transfer (iolink->fd_spi, data_rx, data_tx, size + 2);

}

static uint16_t iolink_cce4511_burst_read_rx (
   iol_cce4511_drv_t * iolink,
   cce4511_channel_t ch,
   uint8_t * data,
   uint8_t rxbytes)
{

	cce4511_reg_addr fhd_reg = cce4511_get_chn_reg_base_addr(ch)
		   + (CCE4511_CHN0_FHD_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);

   uint8_t txdata[IOLINK_RXTX_BUFFER_SIZE + 2] = {0};
   uint8_t rxdata[IOLINK_RXTX_BUFFER_SIZE + 2] = {0};

   uint8_t len = (rxbytes > 31U) ? 31U : rxbytes;

   txdata[0] = (uint8_t) (fhd_reg);
   txdata[1] = (uint8_t)(len << 3);
   txdata[1] |= 1U;

   _iolink_pl_hw_spi_transfer (iolink->fd_spi, rxdata, txdata, rxbytes + 2);
   memcpy (data, &rxdata[2], rxbytes);

   return (uint16_t)((rxdata[0]<<8)|rxdata[1]);
}

static void iolink_cce4511_chip_reset(
		 iol_cce4511_drv_t * iolink){

	 uint8_t reg_val  = 0;

	 reg_val |= CCE4511_REG_CFG_CHIP_RESET_MSK;

	 iolink_cce4511_write_register(iolink, CCE4511_CFG_REG_ADDR, reg_val);
	 while(1){
		 /* Note: CHIP_READY bit is typically 1b1 30μs after reset (in table 36) */
		 /* wait for 30Us */
		 os_usleep(30);
		 reg_val = iolink_cce4511_read_register(iolink, CCE4511_CFG_REG_ADDR);
	     if((reg_val & CCE4511_REG_CFG_CHIP_READY_MSK) != 0) break;
	 }
}

static void iolink_cce4511_configure_cc_register(
		iol_cce4511_drv_t * iolink){
    uint8_t addr, write_val;
    addr = CCE4511_INT_EN_ENV_REG_ADDR;
    write_val = 0x3d;
    iolink_cce4511_write_register(iolink, addr, write_val);
}

static uint8_t iolink_cce4511_configure_channel(
		iol_cce4511_drv_t * iolink,
		cce4511_channel_t channel,
		const uint8_t reg_val[CCE4511_NUM_OF_REG_CHN])
{
	    uint8_t i;
	    cce4511_reg_addr base_address;
	    uint8_t register_value;

	    base_address = cce4511_get_chn_reg_base_addr(channel);

	    os_usleep(500);

	    for(i = 0U; i < CCE4511_NUM_OF_REG_CHN ; i++)
	    {
	    	uint8_t data = reg_val[i];

	        iolink_cce4511_write_register(iolink, base_address + i,  data);
//        	PRINTF("REG @ %x\t WR - %x VAL\r\n", base_address+i, reg_val[i]);
           os_usleep(1000);

	        switch (CCE4511_REG_ADDR_CH_START_ADDRESS + i)
	        {
	        	case CCE4511_CHN0_INT_SRC_STAT_REG_ADDR :
	            case CCE4511_CHN0_INT_SRC_SIO_REG_ADDR :
	            case CCE4511_CHN0_INT_SRC_UART_REG_ADDR :
	            case CCE4511_CHN0_INT_SRC_FH_REG_ADDR :
	            case CCE4511_CHN0_STATUS_REG_ADDR :
	                /* Ignore status register values */
	                continue;
	        }

	        /* Read back configuration registers. */
	        register_value = iolink_cce4511_read_register(iolink, base_address + i);

	        if(CCE4511_REG_ADDR_CH_START_ADDRESS + i == CCE4511_CHN0_SIO_REG_ADDR)
	        {
	            /* The SIO register contains status bit (CQ_IN), which shouldn't be checked. */
	            register_value &= (uint8_t)~CCE4511_REG_SIO_CQ_IN_MSK;

	        }

	        if(reg_val[i] != register_value)
	        {
	        	IOL_LOG_ERR("CHN%d - REG @ %x\tRD - %x & WR - %x VAL DIFF\r\n", channel, base_address+i, register_value, reg_val[i]);
	        }
           os_usleep(100);

	    }
	    return 0;
}

static void iolink_cce4511_set_DO (
   iol_cce4511_drv_t * iolink,
   cce4511_channel_t ch)
{
	uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch);
    uint8_t reg_val = 0xF0;


    os_mutex_lock (iolink->exclusive);
    iolink_cce4511_write_register (iolink, reg_addr, reg_val );
    os_mutex_unlock (iolink->exclusive);

    iolink->is_iolink[ch]    = false;
    iolink->wurq_request[ch] = false;
}

static void iolink_cce4511_set_DI (
		iol_cce4511_drv_t * iolink,
		cce4511_channel_t ch)
{
	uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch);
    uint8_t reg_val = 0xF0;


    os_mutex_lock (iolink->exclusive);
    iolink_cce4511_write_register(iolink, reg_addr, reg_val);
    os_mutex_unlock (iolink->exclusive);

    iolink->is_iolink[ch]    = false;
    iolink->wurq_request[ch] = false;
}

static void iolink_cce4511_set_SDCI (
		iol_cce4511_drv_t * iolink,
		cce4511_channel_t ch)
{
   uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch);

   reg_addr = reg_addr + (CCE4511_CHN0_SIO_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);

   os_mutex_lock (iolink->exclusive);

   iolink_cce4511_write_register (iolink, reg_addr, CCE4511_REG_SIO_STARTUP_SEQ_WURQ);

   os_mutex_unlock (iolink->exclusive);
}

static void iolink_pl_cce4511_wakeup_req(
		struct iolink_hw_drv * iolink_hw,
		void * arg){
	   iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	   cce4511_channel_t ch   = (cce4511_channel_t)arg;
	   iolink_cce4511_set_SDCI(iolink, ch);
}

static void iolink_cce4511_send_master_message (
   iol_cce4511_drv_t * iolink,
   cce4511_channel_t ch)
{
	// no need to trigger anything here automatically message will send
	return;
}

static void iolink_cce4511_delete_master_message (
   iol_cce4511_drv_t * iolink,
   cce4511_channel_t ch)
{
	uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch) + (CCE4511_CHN0_FHC_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);
	uint8_t reg_val  =  CCE4511_REG_FHC_RST_MSK;
	iolink_cce4511_write_register (iolink, reg_addr, reg_val);
}

static void iolink_cce4511_set_master_message(
		iol_cce4511_drv_t * iolink,
		cce4511_channel_t ch,
		uint8_t *         data,
		uint8_t           txlen,
		uint8_t           rxlen,
		uint8_t 		  od_txlen,
		uint8_t 		  od_rxlen,
		bool              keepmessage)
{
	CC_ASSERT (txlen  != 0U);
	CC_ASSERT (txlen  <= IOL_CCE4511_TX_MAX);
	CC_ASSERT (rxlen  != 0U);

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);


	iolink_cce4511_delete_master_message(iolink, ch);

   if (data == NULL || txlen == 0)
   {
      return;
   }
   uint8_t     base_addr = cce4511_get_chn_reg_base_addr(ch);
   mseq_type_t mseq_type = decode_mseq_type(data[1]);

   switch (mseq_type)
   {
   	   case MSEQ_TYPE_0:
   		   write_od_mpd_dpd(iolink, base_addr, 0u, 0u, 0u);
   		   break;

      case MSEQ_TYPE_1:
      case MSEQ_TYPE_2:
	         write_od_mpd_dpd(iolink, base_addr,
	             od_rxlen + od_txlen,
	             (txlen - 2u - od_txlen),
	             (rxlen - od_rxlen - 1u));
          break;
      case MSEQ_TYPE_UNKNOWN:
      default:
         IOL_LOG_ERR ("set_master_message ch%d: unknown CKT in MC=0x%02X\r\n",
                       ch, data[0]);
         write_od_mpd_dpd(iolink, base_addr, 0u, 0u, 0u);
         return;
   }

   iolink_cce4511_burst_write_tx(iolink, ch, data, txlen);
}

static iolink_baudrate_t iolink_pl_cce4511_get_baudrate (
   iolink_hw_drv_t * iolink_hw,
   void * arg)
{
   iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
   cce4511_channel_t ch   = (cce4511_channel_t)arg;

   CC_ASSERT (ch >= CCE4511_CH_MIN);
   CC_ASSERT (ch <= CCE4511_CH_MAX);

   uint8_t reg_val;
   uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch);

   reg_val = iolink_cce4511_read_register(iolink, reg_addr);

   uint8_t comx = (reg_val >> 2) & 0x03;

   switch (comx)
   {
   case 1:
      return IOLINK_BAUDRATE_COM1;
   case 2:
      return IOLINK_BAUDRATE_COM2;
   case 3:
      return IOLINK_BAUDRATE_COM3;
   default:
      return IOLINK_BAUDRATE_NONE;
   }

}

static uint8_t iolink_pl_cce4511_get_cycletime (
   iolink_hw_drv_t * iolink_hw,
   void * arg)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch   = (cce4511_channel_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

	uint8_t reg_val;
	uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch);

	reg_addr = reg_addr + (CCE4511_CHN0_CYCT_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);

	reg_val = iolink_cce4511_read_register (iolink, reg_addr);

   return reg_val;
}

static void iolink_pl_cce4511_set_cycletime (
   iolink_hw_drv_t * iolink_hw,
   void * arg,
   uint8_t cycbyte)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch   = (cce4511_channel_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

	uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch);

	reg_addr = reg_addr + (CCE4511_CHN0_CYCT_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);

	iolink_cce4511_write_register (iolink, reg_addr, cycbyte);
}

static void iolink_pl_cce4511_port_power_off(
		iolink_hw_drv_t * iolink_hw,
		void * arg){
    iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
    cce4511_channel_t ch = (cce4511_channel_t)(uintptr_t)arg;

    CC_ASSERT (ch >= CCE4511_CH_MIN);
    CC_ASSERT (ch < CCE4511_NUM_CHANNELS);

	uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch);

	reg_addr = reg_addr + (CCE4511_CHN0_CFG1_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);

	iolink_cce4511_write_register (iolink, reg_addr, 0x00);

}

static void iolink_pl_cce4511_port_power_on(
		iolink_hw_drv_t * iolink_hw,
		void * arg){
    iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
    cce4511_channel_t ch = (cce4511_channel_t)(uintptr_t)arg;

    CC_ASSERT (ch >= CCE4511_CH_MIN);
    CC_ASSERT (ch < CCE4511_NUM_CHANNELS);

	uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch);

	reg_addr = reg_addr + (CCE4511_CHN0_CFG1_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);

	iolink_cce4511_write_register (iolink, reg_addr, 0xF0);

	os_usleep(300000);

}

static bool iolink_pl_cce4511_set_mode (
   iolink_hw_drv_t * iolink_hw,
   void * arg,
   iolink_pl_mode_t mode)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch   = (cce4511_channel_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

   os_mutex_lock (iolink->exclusive);

   switch (mode)
   {
   case iolink_mode_INACTIVE:
	   iolink_pl_cce4511_port_power_off(iolink_hw, arg);
	   break;
   case iolink_mode_DO:
	  iolink_pl_cce4511_port_power_on(iolink_hw, arg);
      iolink_cce4511_set_DO (iolink, ch);
      break;
   case iolink_mode_DI:
	   iolink_pl_cce4511_port_power_on(iolink_hw, arg);
	   iolink_cce4511_set_DI (iolink, ch);
      break;
   case iolink_mode_SDCI:
	   iolink_pl_cce4511_port_power_on(iolink_hw, arg);
	   iolink_cce4511_set_DI (iolink, ch);

	   iolink->wurq_request[ch] = true;

	   iolink_cce4511_set_SDCI (iolink, ch);
      break;
   }
   os_mutex_unlock (iolink->exclusive);

   return true;
}

static void iolink_pl_cce4511_configure_event (
   iolink_hw_drv_t * iolink_hw,
   void * arg,
   os_event_t * event,
   uint32_t flag)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch   = (cce4511_channel_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

	iolink->dl_event[ch] = event;
	iolink->pl_flag      = flag;
}

static void iolink_pl_cce4511_enable_cycle_timer (
   iolink_hw_drv_t * iolink_hw,
   void * arg)
{
	return;
}

static void iolink_pl_cce4511_disable_cycle_timer (
   iolink_hw_drv_t * iolink_hw,
   void * arg)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch   = (cce4511_channel_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

	uint8_t reg_addr = cce4511_get_chn_reg_base_addr(ch);
	uint8_t reg_val = 0;

	reg_addr = reg_addr + (CCE4511_CHN0_CYCT_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);

	iolink_cce4511_write_register (iolink, reg_addr, reg_val);
}

static void iolink_pl_cce4511_get_error (
   iolink_hw_drv_t * iolink_hw,
   void * arg,
   uint8_t * cqerr)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch   = (cce4511_channel_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

   os_mutex_lock (iolink->exclusive);

   uint8_t base_addr = cce4511_get_chn_reg_base_addr(ch);

   //TransmErrA TCyclErrA TChksmErA TSizeErrA RChksmErA RSizeErrA FrameErrA ParityErrA

  //   DelayErrA BDelayA1 BDelayA0 DDelayA3 DDelayA2 DDelayA1 DDelayA0 RspnsTmrEnA
   uint8_t reg_addr = base_addr + (CCE4511_CHN0_STATUS_MODE_FH_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);
   *cqerr  = iolink_cce4511_read_register (iolink, reg_addr);

   os_mutex_unlock (iolink->exclusive);
}

static bool iolink_pl_cce4511_get_data (
   iolink_hw_drv_t * iolink_hw,
   void * arg,
   uint8_t * rxdata,
   uint8_t len)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch = (cce4511_channel_t)(uintptr_t)arg;

	uint8_t base_addr = cce4511_get_chn_reg_base_addr(ch);

	uint8_t reg_addr = base_addr + (CCE4511_CHN0_BLVL_REG_ADDR - CCE4511_REG_ADDR_CH_START_ADDRESS);

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

   os_mutex_lock (iolink->exclusive);

   uint8_t rxbytes = iolink_cce4511_read_register (iolink, reg_addr);

   if (len < rxbytes)
   {
      IOL_LOG_ERR (
         "%s [ch: %d]: Read buffer too small. Needed: %d. Actual: %d",
         __func__,
         ch,
         rxbytes,
         len);
      rxbytes = len;
   }

   if (rxbytes > 0)
   {
      iolink_cce4511_burst_read_rx (iolink, ch, rxdata, rxbytes);
   }

   os_mutex_unlock (iolink->exclusive);

   return (rxbytes > 0);
}

static void iolink_pl_cce4511_send_msg (iolink_hw_drv_t * iolink_hw, void * arg)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch = (cce4511_channel_t)(uintptr_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

   iolink_cce4511_send_master_message (iolink, ch);
}

static void iolink_pl_cce4511_dl_msg (
   iolink_hw_drv_t * iolink_hw,
   void * arg,
   uint8_t rxbytes,
   uint8_t txbytes,
   uint8_t * data,
   uint8_t od_txlen,
   uint8_t od_rxlen)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch = (cce4511_channel_t)(uintptr_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

   iolink_cce4511_delete_master_message (iolink, ch);
   iolink_cce4511_set_master_message (iolink, ch, data, txbytes, rxbytes, od_txlen, od_rxlen, true);
}

static void iolink_pl_cce4511_transfer_req (
   iolink_hw_drv_t * iolink_hw,
   void * arg,
   uint8_t rxbytes,
   uint8_t txbytes,
   uint8_t * data,
   uint8_t od_txlen,
   uint8_t od_rxlen)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch   = (cce4511_channel_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);

	iolink_cce4511_delete_master_message (iolink, ch);
	iolink_cce4511_set_master_message (iolink, ch, data, txbytes, rxbytes, od_txlen, od_rxlen, true);
	iolink_cce4511_send_master_message (iolink, ch);
}

static bool iolink_pl_cce4511_init_sdci (iolink_hw_drv_t * iolink_hw, void * arg)
{
	iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
	cce4511_channel_t ch   = (cce4511_channel_t)arg;

	CC_ASSERT (ch >= CCE4511_CH_MIN);
	CC_ASSERT (ch <= CCE4511_CH_MAX);


   if (iolink->wurq_request[ch])
   {
      return true;
   }

   iolink_cce4511_set_SDCI(iolink, ch);

   return true;
}

static void iolink_pl_cce4511_pl_handler (
    iolink_hw_drv_t * iolink_hw,
    void * arg)
{
    iol_cce4511_drv_t * iolink = (iol_cce4511_drv_t *)iolink_hw;
    cce4511_channel_t ch = (cce4511_channel_t)(uintptr_t)arg;

    CC_ASSERT (ch >= CCE4511_CH_MIN);
    CC_ASSERT (ch < CCE4511_NUM_CHANNELS);

   (void)ch;

    uint32_t dl_flag          = 0;


    os_mutex_lock(iolink->exclusive);

    uint8_t env_reg = iolink_cce4511_read_register(
                          iolink, CCE4511_INT_SRC_ENV_REG_ADDR);
    if (env_reg & 0x3D)
    {
        IOL_LOG_ERR("PL: INT_SRC_ENV=0x%02X\n", env_reg);

        dl_flag |= IOLINK_PL_EVENT_STATUS;

        if(env_reg & CCE4511_REG_INT_SRC_ENV_IS_T_HIGH_MSK){
        	//todo: port error High temperature
        }
        if((env_reg & CCE4511_REG_INT_SRC_ENV_IS_VDDIO_UV_MSK)| (env_reg & CCE4511_REG_INT_SRC_ENV_IS_VDDA_UV_MSK) |(env_reg & CCE4511_REG_INT_SRC_ENV_IS_VS_UV_MSK)){
        	//todo: port error undervoltage
        }
        if(env_reg & CCE4511_REG_INT_SRC_ENV_IS_VS_OV_MSK){
        	//todo: port error overvoltage
        }
    }

    for (cce4511_channel_t loop_ch = 0;
         loop_ch < CCE4511_NUM_CHANNELS;
         loop_ch++)
    {
        uint8_t base_addr = cce4511_get_chn_reg_base_addr(loop_ch);
        uint8_t reg_addr  = base_addr +
                            (CCE4511_CHN0_INT_SRC_FH_REG_ADDR -
                            		CCE4511_REG_ADDR_CH_START_ADDRESS);

        uint8_t fh_reg = iolink_cce4511_read_register(iolink, reg_addr);

        reg_addr = base_addr +
        			(CCE4511_CHN0_STATUS_REG_ADDR -
        					CCE4511_REG_ADDR_CH_START_ADDRESS);
        uint8_t sts_reg = iolink_cce4511_read_register(iolink, reg_addr);
        uint8_t sts_mask = CCE4511_REG_INT_SRC_STAT_IS_LP_OV_MSK|
        					CCE4511_REG_INT_SRC_STAT_IS_LP_UV_MSK|
							CCE4511_REG_INT_SRC_STAT_IS_CH_DIS_LPX_CLA_MSK|
							CCE4511_REG_INT_SRC_STAT_IS_CH_DIS_LPX_OVC_MSK;


        if(sts_reg & sts_mask){
        	//todo: set the port error
        }
//        IOL_LOG_DBG("pl_handler ch%d: INT_SRC_FH=0x%02X\n", loop_ch, fh_reg);

        if (fh_reg == 0)
            continue;

        if (fh_reg & CCE4511_REG_INT_SRC_FH_IS_WURQ_MSK)
        {
            if (iolink->wurq_request[loop_ch])
            {
                dl_flag |= IOLINK_PL_EVENT_WURQ;
            }
            else
            {
                IOL_LOG_WRN("pl_handler ch%d: WURQ no request pending\n",
                            loop_ch);
            }
        }

        if (fh_reg & CCE4511_REG_INT_SRC_FH_IS_MSG_MSK)
        {
//            IOL_LOG_DBG("pl_handler ch%d: RXRDY\n", loop_ch);

            dl_flag |= IOLINK_PL_EVENT_RXRDY;
        }

        if (fh_reg & CCE4511_REG_INT_SRC_FH_IS_ERR_MSK)
        {
//            IOL_LOG_DBG("pl_handler ch%d: RXERR\n", loop_ch);
            dl_flag |= IOLINK_PL_EVENT_RXERR;
        }


        if (fh_reg & CCE4511_REG_INT_SRC_FH_IS_TOUT_MSK)
        {
//            IOL_LOG_INF("\r\npl_handler[%d]: Timeout fh=0x%02X\n",loop_ch, fh_reg);
//             dl_flag |= IOLINK_DL_EVENT_TIMEOUT;
            dl_flag |=IOLINK_PL_EVENT_RXERR;
        }
        if (fh_reg & CCE4511_REG_INT_SRC_FH_IS_EOC_MSK)
        {
//            IOL_LOG_INF("\r\npl_handler[%d]: Timeout tcyc fh=0x%02X\n",loop_ch, fh_reg);
//             dl_flag |= IOLINK_DL_EVENT_TIMEOUT_TCYC;
            dl_flag |=IOLINK_PL_EVENT_RXERR;
        }

        if (dl_flag != 0)
        {
//            IOL_LOG_DBG("pl_handler ch%d: posting dl_flag=0x%08lX\n",loop_ch, (unsigned long)dl_flag);
            os_event_set(iolink->dl_event[loop_ch], dl_flag);
        }
    }

    os_mutex_unlock(iolink->exclusive);
}

static const iolink_hw_ops_t iolink_hw_ops = {
   .get_baudrate        = iolink_pl_cce4511_get_baudrate,
   .get_cycletime       = iolink_pl_cce4511_get_cycletime,
   .set_cycletime       = iolink_pl_cce4511_set_cycletime,
   .set_mode            = iolink_pl_cce4511_set_mode,
   .wakeup_req          = iolink_pl_cce4511_wakeup_req,
   .enable_cycle_timer  = iolink_pl_cce4511_enable_cycle_timer,
   .disable_cycle_timer = iolink_pl_cce4511_disable_cycle_timer,
   .get_error           = iolink_pl_cce4511_get_error,
   .get_data            = iolink_pl_cce4511_get_data,
   .send_msg            = iolink_pl_cce4511_send_msg,
   .dl_msg              = iolink_pl_cce4511_dl_msg,
   .transfer_req        = iolink_pl_cce4511_transfer_req,
   .init_sdci           = iolink_pl_cce4511_init_sdci,
   .configure_event     = iolink_pl_cce4511_configure_event,
   .pl_handler          = iolink_pl_cce4511_pl_handler,
   .port_power_off 	    = iolink_pl_cce4511_port_power_off,
   .port_power_on		= iolink_pl_cce4511_port_power_on,
};

iolink_hw_drv_t * iolink_cce4511_init (const iolink_cce4511_cfg_t * cfg)
{
	iol_cce4511_drv_t * iolink;
   uint8_t ch;
   uint8_t rev;

   /* Allocate driver structure */
   iolink = calloc (1, sizeof (iol_cce4511_drv_t));
   if (iolink == NULL)
   {
      return NULL;
   }

   g_iolink_drv = iolink;

   /* Initialise driver structure */
   iolink->drv.ops      = &iolink_hw_ops;
   for (ch = 0; ch < CCE4511_NUM_CHANNELS; ch++)
   {
      iolink->wurq_request[ch] = false;
   }
   iolink->fd_spi = _iolink_pl_hw_spi_init(cfg->spi_slave_name);

   if (iolink->fd_spi == NULL)
   {
      IOL_LOG_ERR ("PL: Unable to open spi device: %s\n", cfg->spi_slave_name);
      free (iolink);
      return NULL;
   }

   iolink->exclusive = os_mutex_create();
   iolink->drv.mtx = iolink->exclusive;

   uint8_t retries = 100;

   while (retries--)
   {
       rev = iolink_cce4511_read_register(iolink, CCE4511_REV_REG_ADDR);
       if (rev == REVID_CCE4511) break;

       IOL_LOG_WRN("PL: CHIP REV ID=0x%02X, waiting for spi ready...\n", rev);
       os_usleep(500000);
   }

   if (rev != REVID_CCE4511)
   {
       IOL_LOG_ERR("PL: Unsupported chip revision: 0x%02x\n", rev);
       os_mutex_destroy(iolink->exclusive);
       _iolink_pl_hw_spi_close(iolink->fd_spi);
       free(iolink);
       return NULL;
   }

   iolink_cce4511_chip_reset(iolink);

   iolink_cce4511_configure_cc_register(iolink);

   for (ch = 0; ch < CCE4511_NUM_CHANNELS; ch++)
   {
	   if(iolink_cce4511_configure_channel (iolink, ch, reg_vals)){
		   IOL_LOG_ERR ("PL: iolink_cce4511_configure_channel error\r\n");
		   os_mutex_destroy(iolink->exclusive);
		   _iolink_pl_hw_spi_close(iolink->fd_spi);
		   free (iolink);
	   }
   }

   if (cfg->register_read_reg_fn != NULL)
   {
      cfg->register_read_reg_fn (iolink_cce4511_read_register);
   }

   return &iolink->drv;
}


/**
 * Interrupt service routine for the iolink_max14819 driver instance.
 *
 * @param arg     Reference to the driver instance
 */
void iolink_cce4511_isr (void * arg)
{
	iol_cce4511_drv_t * iolink;
	iolink = (iol_cce4511_drv_t *)arg;
   uint8_t ch;

	for(ch = 0; ch < CCE4511_NUM_CHANNELS; ch++){

	    if (iolink->dl_event[ch] != NULL)
	    {
	    	os_event_set (iolink->dl_event[ch], iolink->pl_flag);
	    }
	}

}

/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/
