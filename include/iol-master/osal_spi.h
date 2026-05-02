/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * iol_osal/include/osal_spi.h
 *
 * IO-Link OSAL SPI interface — Zephyr port.
 */

#ifndef OSAL_PL_HW_SPI_H_INCLUDED
#define OSAL_PL_HW_SPI_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward-declare to avoid pulling <zephyr/drivers/spi.h> into every
 * stack source that includes this header. */
struct spi_dt_spec;

/**
 * @brief Inject the Zephyr SPI device spec before calling
 *        _iolink_pl_hw_spi_init().
 *
 * Typical usage in main.c / board_init.c:
 *
 *   #include <zephyr/drivers/spi.h>
 *   #include <osal_spi.h>
 *
 *   static const struct spi_dt_spec cce4511_spi =
 *       SPI_DT_SPEC_GET(DT_NODELABEL(cce4511_0), CCE4511_SPI_OPERATION, 0);
 *
 *   iol_spi_set_device(&cce4511_spi);
 */
void iol_spi_set_device(const struct spi_dt_spec *dev);

/**
 * @brief Initialise the SPI bus for the CCE4511.
 * @param spi_slave_name  Ignored in the Zephyr port.
 * @return Opaque handle (cast of spi_dt_spec *), or NULL on error.
 */
void *_iolink_pl_hw_spi_init(const char *spi_slave_name);

/**
 * @brief Release the SPI bus resources.
 * @param fd  Handle returned by _iolink_pl_hw_spi_init().
 */
void _iolink_pl_hw_spi_close(void *fd);

/**
 * @brief Full-duplex SPI transfer.
 * @param fd                   Handle from _iolink_pl_hw_spi_init().
 * @param data_read            RX buffer (NULL for TX-only).
 * @param data_written         TX buffer (NULL for RX-only).
 * @param n_bytes_to_transfer  Byte count for the exchange.
 */
void _iolink_pl_hw_spi_transfer(
    void       *fd,
    void       *data_read,
    const void *data_written,
    size_t      n_bytes_to_transfer);

#ifdef __cplusplus
}
#endif

#endif /* OSAL_PL_HW_SPI_H_INCLUDED */