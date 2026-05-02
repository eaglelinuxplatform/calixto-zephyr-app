/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * src/osal/osal_irq_stub.c — Stub for the OSAL IRQ helper.
 */

#include "iol-master/osal_irq.h"
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(iol_osal, CONFIG_IOL_MASTER_LOG_LEVEL);

int _iolink_setup_int(int gpio_pin, isr_func_t isr_func, void *irq_arg)
{
    /*
     * Not implemented in the Zephyr port.
     * GPIO interrupt setup is handled in main.c via:
     *   gpio_pin_interrupt_configure_dt()
     *   gpio_add_callback()
     */
    ARG_UNUSED(gpio_pin);
    ARG_UNUSED(isr_func);
    ARG_UNUSED(irq_arg);

    LOG_ERR("_iolink_setup_int() called — this function is not implemented "
            "in the Zephyr port.  Use the Zephyr GPIO API directly.");
    return -ENOSYS;
}
