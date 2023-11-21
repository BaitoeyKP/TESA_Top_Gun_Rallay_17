#include "main.h"
#include "task_period.h"
#include "net_mqtt.h"

// constants
#define TAG         "task_period"

#define BTN_PIN     0 

// static function prototypes
static void task_period_fcn(void* arg);

// task initialization
void task_period_init(uint32_t period_ms) {
    static uint32_t arg = period_ms;
    xTaskCreate(
        ,    /* Task function. */
        ,    /* String with name of task. */
        ,               /* Stack size in bytes. */
        ,               /* Parameter passed as input of the task */
        ,   /* Priority of the task. */
        );              /* Task handle. */
    ESP_LOGI(TAG, "task_period created at %d", millis());
}

// task function
void task_period_fcn(void* arg) {
    // task initialization
    uint32_t period_ms = *((uint32_t*)arg);
    while(1) {
        // task function
        evt_msg_t evt_msg = {
            .type = TASK_PERIOD_TYPE,
            .timestamp = millis(),
            .pressed = false,
            .value = esp_random()
        };
        ESP_LOGI(TAG, "task_period run at %d", millis());
        // send event and delay
        
    }
}
