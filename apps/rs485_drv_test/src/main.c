#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include <zephyr/devicetree.h>
#include <drivers/rs_485.h>

#define RS485_NODE DT_NODELABEL(rs485_calixto_0)
#define RS485_NODE1 DT_NODELABEL(rs485_calixto_1)

void user_uart_callback(const struct device *dev, void *user_data)
{
    
    if (uart_irq_rx_ready(dev)) {
        uint8_t data;
        while (uart_fifo_read(dev, &data, 1) == 1) {
            printk("Received: %c \n", data);
        }
    }

}

int main(void)
{
    const struct device *rs485_dev = DEVICE_DT_GET(RS485_NODE);
    const struct device *rs485_dev1 = DEVICE_DT_GET(RS485_NODE1);
    
    if (!device_is_ready(rs485_dev)) {
        printk("RS-485 device not ready\n");
        return 1;
    }
    if (!device_is_ready(rs485_dev1)) {
        printk("RS-485 device 1 not ready\n");
        return 1;
    }
    printk("RS-485 devices ready\n");


    int ret = rs485_callback_register(rs485_dev, user_uart_callback, NULL);
    if(ret < 0) {
		if (ret == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
		} else if (ret == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
		} else {
			printk("Error setting UART callback: %d\n", ret);
		}
		return 0;
    }
    ret = rs485_callback_register(rs485_dev1, user_uart_callback, NULL);
    if(ret < 0) {
		if (ret == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
		} else if (ret == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
		} else {
			printk("Error setting UART callback: %d\n", ret);
		}
		return 0;
    }

    const uint8_t data[] = "Hello RS-485 from uart2!";
    ret = rs485_transmit(rs485_dev, data, sizeof(data));
    if (ret < 0) {
        printk("Failed to transmit data over RS-485: %d\n", ret);
        return 1;
    }

    const uint8_t data2[] = "Hello RS-485 from uart6!";

    ret = rs485_transmit(rs485_dev1, data2, sizeof(data2));
    if (ret < 0) {
        printk("Failed to transmit data over RS-485: %d\n", ret);
        return 1;
    }

    while(1){
        ret = rs485_transmit(rs485_dev, data, sizeof(data));
        if (ret < 0) {
            printk("Failed to transmit data over RS-485: %d\n", ret);
        }   
        k_msleep(5000);
    }

	return 0;
}
