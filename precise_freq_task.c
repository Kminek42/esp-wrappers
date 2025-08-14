#include "precise_freq_task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gptimer.h"
#include "esp_err.h"

static SemaphoreHandle_t timer_semaphore = NULL;
static TaskFunction_t user_callback = NULL;

static bool IRAM_ATTR timer_isr_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_ctx) {
    BaseType_t high_task_woken = pdFALSE;
    if (timer_semaphore != NULL) {
        xSemaphoreGiveFromISR(timer_semaphore, &high_task_woken);
    }
    return high_task_woken == pdTRUE;
}

static void timer_task(void* pvParameters) {
    while (1) {
        if (xSemaphoreTake(timer_semaphore, portMAX_DELAY) == pdTRUE) {
            if (user_callback != NULL) {
                user_callback(pvParameters);
            }
        }
    }
}

esp_err_t precise_freq_task_init(TaskFunction_t callback, uint32_t frequency_hz,
    uint32_t stack_depth, UBaseType_t priority, BaseType_t core_id) {

    if (callback == NULL || frequency_hz == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    user_callback = callback;

    // Create the binary semaphore
    timer_semaphore = xSemaphoreCreateBinary();
    if (timer_semaphore == NULL) {
        return ESP_FAIL;
    }

    // Timer configuration
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1 MHz resolution
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    // Alarm configuration
    uint64_t alarm_count = timer_config.resolution_hz / frequency_hz;
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = alarm_count,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    // Register ISR and start timer
    gptimer_event_callbacks_t cb = { .on_alarm = timer_isr_callback };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cb, NULL));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    // Create the task
    BaseType_t result = xTaskCreatePinnedToCore(timer_task, "TimerTask", stack_depth, NULL, priority, NULL, core_id);
    if (result != pdPASS) {
        return ESP_FAIL;
    }

    return ESP_OK;
}