/*******************************************************************************
  * @file    cce4511_conf.h
  * @author  Sanjana S, Calixto Firmware Team
  * @version V2.0.0
  * @date    17-Apr-2026
  * @brief   CCE4511 compile-time configuration constants, Zephyr port.
  *
  * CHANGES FROM V1.0.0 (NXP/FreeRTOS version)
  * ──────────────────────────────────────────
  * REMOVED  CCE4511_SPI        LPSPI4          — NXP peripheral reference;
  *                                               causes "undeclared identifier"
  *                                               in any Zephyr translation unit
  *                                               that includes this header.
  *
  * REMOVED  CCE4511_INIT_CS    kLPSPI_Pcs0    — NXP SDK enum; same issue.
  *
  * REMOVED  CCE4511_CS_GPIO    GPIO_11, 16    — NXP GPIO macro format; unused
  *                                               in Zephyr (CS is managed by
  *                                               the SPI driver via cs-gpios
  *                                               in the device tree overlay).
  *
  * ADDED    CCE4511_SPI_OPERATION              — Zephyr SPI operation flags
  *                                               for use with SPI_DT_SPEC_GET
  *                                               in main.c.  Consolidating the
  *                                               definition here (rather than
  *                                               in main.c) keeps it in sync
  *                                               with CCE4511_BAUDRATE and the
  *                                               rest of the chip config.
  *
  * KEPT     CCE4511_REG_WRITE_RETRY_COUNT      — chip-level constant, unchanged.
  * KEPT     CCE4511_BAUDRATE                   — renamed from CCE4511_BARDRATE
  *                                               (typo fix; update references).
  *
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************/

#ifndef CCE4511_CONF_H_INCLUDED
#define CCE4511_CONF_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <zephyr/drivers/spi.h>   /* SPI_OP_MODE_MASTER, SPI_WORD_SET, etc. */

/* ---------------------------------------------------------------------------
 * SPI bus parameters
 * ---------------------------------------------------------------------------*/

/** Maximum SPI clock frequency supported by the CCE4511 (Hz). */
#define CCE4511_BAUDRATE    2500000U

/**
 * @brief Zephyr SPI operation flags for the CCE4511.
 *
 * - SPI_OP_MODE_MASTER : this side drives the clock
 * - SPI_WORD_SET(8)    : 8-bit word size
 * - SPI_TRANSFER_MSB   : MSB transmitted first
 *
 * Mode 0 (CPOL=0, CPHA=0) is the default when neither SPI_MODE_CPOL
 * nor SPI_MODE_CPHA is OR-ed in — matches the CCE4511 datasheet.
 *
 * Usage in main.c:
 *   static const struct spi_dt_spec cce4511_spi =
 *       SPI_DT_SPEC_GET(CCE4511_NODE, CCE4511_SPI_OPERATION, 0);
 */
#define CCE4511_SPI_OPERATION \
    (SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB)

/* ---------------------------------------------------------------------------
 * Driver tunables
 * ---------------------------------------------------------------------------*/

/** Number of times a register write is retried before giving up. */
#define CCE4511_REG_WRITE_RETRY_COUNT   5U

#ifdef __cplusplus
}
#endif

/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/

#endif /* CCE4511_CONF_H_INCLUDED */
