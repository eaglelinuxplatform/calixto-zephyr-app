/*
 * Copyright (c) 2026 Calixto Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#define LED0 DT_ALIAS(led0)


#define GPI_INT_NODE  DT_NODELABEL(gpio2)
#define GPI_INT_PIN1  11
#define GPI_INT_PIN2  13
#define GPI_INT_PIN3  14

#define GPO_NODE  DT_NODELABEL(gpio8)
#define GPO_PIN1  17



#define SLEEP_MS 1000

const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0, gpios);

struct gpio_callback gpio_int_cb;


static void gpio_int_callback(const struct device *gpio_dev,
                               struct gpio_callback *cb,
                               gpio_port_pins_t pins)
{
    ARG_UNUSED(cb);

    if (pins & BIT(GPI_INT_PIN1)) {
        printk("INT: Port=%s, Pin=%d\r\n",
               gpio_dev->name,
               GPI_INT_PIN1);
    }

    if (pins & BIT(GPI_INT_PIN2)) {
        printk("INT: Port=%s, Pin=%d\r\n",
               gpio_dev->name,
               GPI_INT_PIN2);
    }

    if (pins & BIT(GPI_INT_PIN3)) {
        printk("INT: Port=%s, Pin=%d\r\n",
               gpio_dev->name,
               GPI_INT_PIN3);
    }
}

int main(void)
{
	printf("\r\nGPIO SAMPLE\r\n");

  const struct device *gpi_int_dev = DEVICE_DT_GET(GPI_INT_NODE);
  if(!device_is_ready(gpi_int_dev)){
      printk("gpio1 is not ready\r\n");
  }

  const struct device *gpo_dev = DEVICE_DT_GET(GPO_NODE);
  if(!device_is_ready(gpo_dev)){
      printk("gpio1 is not ready\r\n");
  }

  int ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
  if(ret < 0){
    return 1;
  }

	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);

  ret = gpio_pin_configure(gpi_int_dev, GPI_INT_PIN1, (GPIO_INPUT| GPIO_PULL_UP) );
  ret = gpio_pin_configure(gpi_int_dev, GPI_INT_PIN2, (GPIO_INPUT | GPIO_PULL_UP) );
  ret = gpio_pin_configure(gpi_int_dev, GPI_INT_PIN3, (GPIO_INPUT | GPIO_PULL_UP) );

  ret = gpio_pin_configure(gpo_dev, GPO_PIN1, (GPIO_OUTPUT_ACTIVE) );

  gpio_init_callback(&gpio_int_cb, gpio_int_callback, (BIT(GPI_INT_PIN1) | BIT(GPI_INT_PIN2) | BIT(GPI_INT_PIN3) ));
  ret = gpio_add_callback(gpi_int_dev, &gpio_int_cb);

  gpio_pin_interrupt_configure(gpi_int_dev, GPI_INT_PIN1, GPIO_INT_EDGE_FALLING );
  gpio_pin_interrupt_configure(gpi_int_dev, GPI_INT_PIN2, GPIO_INT_EDGE_FALLING );
  gpio_pin_interrupt_configure(gpi_int_dev, GPI_INT_PIN3, GPIO_INT_EDGE_FALLING );

  gpio_pin_set(gpo_dev, GPO_PIN1, 0); //GPIO_LOW

  gpio_pin_set(gpo_dev, GPO_PIN1, 1); //GPIO_HIGH

	while(1){
    ret = gpio_pin_toggle_dt(&led0);
    if (ret < 0) {
      return 0;
    }
    k_msleep(SLEEP_MS);
  }
	return 0;
}
