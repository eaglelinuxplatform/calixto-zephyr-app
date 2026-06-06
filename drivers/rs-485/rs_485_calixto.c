#define DT_DRV_COMPAT calixto_rs_485

#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "drivers/rs_485.h"

LOG_MODULE_REGISTER(rs_485_calixto, CONFIG_RS_485_LOG_LEVEL);

struct rs_485_config {
    const struct device *uart_dev;
    struct gpio_dt_spec gpio;
};

static int _rs_485_transmit(const struct device *dev, const uint8_t *data, size_t len){
        LOG_INF("Transmitting data over RS-485");
        const struct rs_485_config *cfg = dev->config;

        /*Write a character to the device for output.
        This routine checks if the transmitter is full. 
        When the transmitter is not full,
         it writes a character to the data register. 
         It waits and blocks the calling thread, otherwise. 
         This function is a blocking call*/
         
        gpio_pin_set_dt(&cfg->gpio, 0);
        for (size_t i = 0; i < len; i++) {  
            uart_poll_out(cfg->uart_dev, data[i]);
        }
        gpio_pin_set_dt(&cfg->gpio, 1);

        return 0;
}

static int _rs_485_callback_register(const struct device *dev, uart_irq_callback_user_data_t callback, void *user_data){
        LOG_DBG("Registering callback for RS-485 events");

    const struct rs_485_config *cfg = dev->config;
    uart_irq_rx_enable(cfg->uart_dev);

    return uart_irq_callback_user_data_set(cfg->uart_dev, callback, user_data);
}

static int rs_485_init(const struct device *dev)
{
    LOG_DBG("Initializing RS-485 driver");

    const struct rs_485_config *cfg = dev->config;

    if(!device_is_ready(cfg->uart_dev)) {
        LOG_ERR("UART device not ready");
        return -ENODEV;
    }

    if (!device_is_ready(cfg->gpio.port)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    gpio_pin_configure_dt(&cfg->gpio, GPIO_OUTPUT_INACTIVE);
    return 0;
}

static const struct rs_485_driver_api rs485_api = {
    .transmit          = &_rs_485_transmit,
    .callback_register = &_rs_485_callback_register,
};

#define RS_485_DEVICE(inst)                                              \
    static struct rs_485_config rs_485_config_##inst = {                 \
        .uart_dev = DEVICE_DT_GET(DT_BUS(DT_DRV_INST(inst))),            \
        .gpio = GPIO_DT_SPEC_INST_GET(inst, rs_485_gpio),               \
    };                                                                   \
                                                                         \
    DEVICE_DT_INST_DEFINE(inst,                                          \
                          rs_485_init,                                   \
                          NULL,                                          \
                          NULL,                                          \
                          &rs_485_config_##inst,                         \
                          POST_KERNEL,                                   \
                          CONFIG_KERNEL_INIT_PRIORITY_DEVICE,            \
                          &rs485_api);

DT_INST_FOREACH_STATUS_OKAY(RS_485_DEVICE)    