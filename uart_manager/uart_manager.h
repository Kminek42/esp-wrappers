#pragma once
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"

void uart_manager_init(void);
void uart_manager_read_instruction(void (*callback)(uint8_t parameter, uint8_t value));
void uart_manager_send_instruction(uint8_t parameter, uint8_t value);
