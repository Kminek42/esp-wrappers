#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

/**
 * @brief Initializes and starts a hardware timer to periodically execute a task.
 *
 * @param callback The function to be executed by the timer task.
 * @param frequency_hz The desired execution frequency in Hertz.
 * @param stack_depth The stack size for the timer's dedicated task.
 * @param priority The priority of the timer's dedicated task.
 * @param core_id The core on which to run the task (0 or 1).
 *
 * @return esp_err_t ESP_OK on success, otherwise an error code.
 */
esp_err_t precise_freq_task_init(TaskFunction_t callback, uint32_t frequency_hz,
    uint32_t stack_depth, UBaseType_t priority, BaseType_t core_id);