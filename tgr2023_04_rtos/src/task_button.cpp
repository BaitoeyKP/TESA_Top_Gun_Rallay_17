#include "main.h"
#include "task_button.h"
#include "net_mqtt.h"

// constants
#define TAG         "task_button"

#define BTN_PIN     0

// static function prototypes
static void task_button_fcn(void* arg);

// task initialization
void task_button_init() {
    // fill task parameters
    xTaskCreate(
        ,    /* Task function. */
        ,      /* String with name of task. */
        ,               /* Stack size in bytes. */
        ,               /* Parameter passed as input of the task */
        ,   /* Priority of the task. */
        );              /* Task handle. */
    ESP_LOGI(TAG, "task_button created at %d", millis());
}

// task function
void task_button_fcn(void* arg) {
    uint32_t prev_millis = millis();
    // task initialization
    pinMode(BTN_PIN, INPUT_PULLUP);
    while(1) {
        // task function
        evt_msg_t evt_msg = {
            .type = TASK_BUTTON_TYPE,
            .timestamp = 0,
            .pressed = false,
            .value = 0
        };
        if (enable_flag) {
            if (digitalRead(BTN_PIN) == LOW) {
                ESP_LOGI(TAG, "Button pressed at %d", millis());
                if (millis() - prev_millis > 500) {
                    prev_millis = millis();
                    // generate and send event

                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}