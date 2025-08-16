#include "uart_manager.h"

#define UART_NUM UART_NUM_1
#define UART_BUF_SIZE 1024

void uart_manager_init(uart_manager_config_t config) {
    const uart_config_t uart_config = {
        .baud_rate = 31250,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(UART_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, config.tx_pin, config.rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int uart_manager_receive_data(uint8_t *data, size_t length) {
    return uart_read_bytes(UART_NUM, data, 1, portMAX_DELAY);
}

void uart_manager_send_instruction(uint8_t parameter, uint8_t value) {
    uint8_t data[2] = { 0 };
    data[0] = parameter + 128;
    data[1] = value;
    uart_write_bytes(UART_NUM, (const char*)data, 2);
}

void uart_manager_read_instruction(void (*callback)(uint8_t parameter, uint8_t value)) {
    static uint8_t parameter = 0;
    static uint8_t value = 0;

    uint8_t data = 0;

    while (uart_read_bytes(UART_NUM, &data, 1, portMAX_DELAY)) {
        if (data >= 128) {
            parameter = data - 128;
        }
        else {
            value = data;
            callback(parameter, value);
        }
    }
}
