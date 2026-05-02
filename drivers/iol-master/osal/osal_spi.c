/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * src/osal/osal_spi.c — SPI OSAL implementation for the IO-Link Master,
 * Zephyr port.
 *
 */

#include "iol-master/osal_spi.h"
#include "iol-master/osal_spi_internal.h"
#include "iol-master/cce4511_conf.h"
#include "iol-master/iol_log.h"

#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <stdint.h>
#include <stddef.h>

/* ── Module-level state ─────────────────────────────────────────────── */

/* Mutex shared with osal_irq.c for SPI bus exclusion during transfers. */
os_mutex_t *spi_io_mutex;

/*
 * Pointer to the SPI device spec injected by the application via
 * iol_spi_set_device().  The spec holds the Zephyr SPI device pointer,
 * bus configuration (frequency, mode, word size), and CS GPIO reference
 * — everything needed by spi_transceive_dt().
 */
static const struct spi_dt_spec *s_spi_dev;

/* ── Public: device injection ────────────────────────────────────────── */

/**
 * @brief Provide the SPI device spec to use for all CCE4511 transfers.
 *
 * Must be called once before _iolink_pl_hw_spi_init().  Typical usage:
 *
 *   static const struct spi_dt_spec my_spi =
 *       SPI_DT_SPEC_GET(DT_NODELABEL(cce4511_0), CCE4511_SPI_OPERATION, 0);
 *   iol_spi_set_device(&my_spi);
 */
void iol_spi_set_device(const struct spi_dt_spec *dev)
{
    s_spi_dev = dev;
}

/* ── OSAL SPI interface ───────────────────────────────────────────────── */

/**
 * @brief Initialise the SPI bus for the CCE4511.
 *
 * In the Zephyr port the SPI controller is fully configured by the DTS
 * node and the Zephyr SPI driver — no manual register setup is required.
 * This function validates that the device is ready and creates the bus mutex.
 *
 * @param spi_slave_name  Ignored in the Zephyr port.  The device is
 *                        supplied via iol_spi_set_device().
 * @return Pointer to the spi_dt_spec on success, NULL on error.
 */
void *_iolink_pl_hw_spi_init(const char *spi_slave_name)
{
    ARG_UNUSED(spi_slave_name);

    if (s_spi_dev == NULL) {
        IOL_LOG_ERR("osal_spi: call iol_spi_set_device() before init");
        return NULL;
    }

    if (!spi_is_ready_dt(s_spi_dev)) {
        IOL_LOG_ERR("osal_spi: SPI device not ready");
        return NULL;
    }

    spi_io_mutex = os_mutex_create();
    if (spi_io_mutex == NULL) {
        IOL_LOG_ERR("osal_spi: failed to create SPI mutex");
        return NULL;
    }

    IOL_LOG_INF("osal_spi: SPI bus ready @ %u Hz",
                s_spi_dev->config.frequency);

    return (void *)s_spi_dev;
}

/**
 * @brief Release the SPI bus.
 *
 * @param fd  Handle returned by _iolink_pl_hw_spi_init().
 */
void _iolink_pl_hw_spi_close(void *fd)
{
    ARG_UNUSED(fd);

    /*
     * Zephyr SPI drivers do not have an explicit de-init call in the
     * generic API.  If your specific driver exposes one, add it here.
     */
    if (spi_io_mutex != NULL) {
        os_mutex_destroy(spi_io_mutex);
        spi_io_mutex = NULL;
    }

    s_spi_dev = NULL;
    IOL_LOG_INF("osal_spi: SPI bus closed");
}

/**
 * @brief Perform a full-duplex SPI transfer.
 *
 * Replaces LPSPI_MasterTransferBlocking() + taskENTER/EXIT_CRITICAL.
 * Thread safety is achieved via the os_mutex (correct for Zephyr threads).
 * The old critical-section approach was wrong for a multi-threaded RTOS:
 * it would have disabled ALL interrupts system-wide, not just the bus.
 *
 * @param fd              Handle from _iolink_pl_hw_spi_init().
 * @param data_read       Buffer for received bytes (may be NULL for TX-only).
 * @param data_written    Buffer of bytes to transmit (may be NULL for RX-only).
 * @param n_bytes_to_transfer  Number of bytes to exchange.
 */
void _iolink_pl_hw_spi_transfer(
    void       *fd,
    void       *data_read,
    const void *data_written,
    size_t      n_bytes_to_transfer)
{
    const struct spi_dt_spec *spi = (const struct spi_dt_spec *)fd;

    if (spi == NULL) {
        IOL_LOG_ERR("osal_spi: transfer called with NULL fd");
        return;
    }

    /*
     * Build Zephyr SPI buffer sets.
     * spi_transceive_dt() does a simultaneous TX+RX in one transaction.
     * The Zephyr driver asserts CS at the start and deasserts at the end
     * automatically (from the DTS cs-gpios definition).
     */
    const struct spi_buf tx_buf = {
        .buf = (void *)data_written,   /* cast away const: Zephyr API quirk */
        .len = n_bytes_to_transfer,
    };
    struct spi_buf rx_buf = {
        .buf = data_read,
        .len = n_bytes_to_transfer,
    };

    /*
     * Allow NULL TX or RX for one-directional transfers.
     * spi_transceive_dt() accepts NULL buffer sets.
     */
    const struct spi_buf_set tx_set = {
        .buffers = (data_written != NULL) ? &tx_buf : NULL,
        .count   = (data_written != NULL) ? 1U       : 0U,
    };
    const struct spi_buf_set rx_set = {
        .buffers = (data_read != NULL) ? &rx_buf : NULL,
        .count   = (data_read != NULL) ? 1U      : 0U,
    };

    os_mutex_lock(spi_io_mutex);

    int rc = spi_transceive_dt(spi, &tx_set, &rx_set);
    if (rc != 0) {
        IOL_LOG_ERR("osal_spi: spi_transceive_dt failed (%d)", rc);
    }

    os_mutex_unlock(spi_io_mutex);
}