#ifndef OSAL_SPI_HELPERS_H
#define OSAL_SPI_HELPERS_H

#include "osal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A Mutex lock used when accessing the ftdi_handle during communication with
 * the chip.*/

extern os_mutex_t * spi_io_mutex;


#ifdef __cplusplus
}
#endif

#endif
