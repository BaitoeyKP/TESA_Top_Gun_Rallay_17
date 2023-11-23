#include "main.h"
#include "task_camera.h"
#include "hw_camera.h"
#include "net_mqtt.h"
#include "detection.h"

// constants
#define TAG "task_camera"

#define BTN_PIN 0

// static function prototypes
static void task_camera_fcn(void *arg);

// task initialization
void task_camera_init()
{
    xTaskCreate(
        task_camera_fcn,  /* Task function. */
        "Camera Task",    /* String with name of task. */
        2048,             /* Stack size in bytes. */
        NULL,             /* Parameter passed as input of the task */
        TASK_CAMERA_PRIO, /* Priority of the task. */
        NULL);            /* Task handle. */
    ESP_LOGI(TAG, "task_camera created at %d", millis());
}

// task function
void task_camera_fcn(void *arg)
{
    // task initialization
    while (1)
    {
        print_memory();
        pinMode(BTN_PIN, INPUT_PULLUP);
        hw_camera_init();
        bmp_buf = (uint8_t *)ps_malloc(BMP_BUF_SIZE);
        if (psramInit())
        {
            ESP_LOGI(TAG, "PSRAM initialized");
        }
        else
        {
            ESP_LOGE(TAG, "PSRAM not available");
        }
        // task function
        evt_msg_t evt_msg = {
            .type = TASK_CAMERA_TYPE,
            .id = MQTT_DEV_ID};

        static bool press_state = false;
        static uint32_t prev_millis = 0;

        if (digitalRead(BTN_PIN) == 0)
        {
            if ((millis() - prev_millis > 500) && (press_state == false))
            {
                uint32_t Tstart, elapsed_time;
                uint32_t width, height;

                prev_millis = millis();
                Tstart = millis();
                // get raw data
                ESP_LOGI(TAG, "Taking snapshot...");
                // use raw bmp image
                hw_camera_raw_snapshot(bmp_buf, &width, &height);
                elapsed_time = millis() - Tstart;
                ESP_LOGI(TAG, "Snapshot taken (%d) width: %d, height: %d", elapsed_time, width, height);
                print_memory();
                // prepare feature
                Tstart = millis();
                ei::signal_t signal;
                // generate feature
                ei_prepare_feature(bmp_buf, &signal);
                elapsed_time = millis() - Tstart;
                ESP_LOGI(TAG, "Feature taken (%d)", elapsed_time);
                print_memory();
                // run classifier
                Tstart = millis();
                ei_impulse_result_t result = {0};
                bool debug_nn = false;
                // run classifier
                run_classifier(&signal, &result, debug_nn);
                elapsed_time = millis() - Tstart;
                ESP_LOGI(TAG, "Classification done (%d)", elapsed_time);
                print_memory();
                // use result
                ei_use_result(result);
                press_state = true;
            }
            // evt_msg.id = label;
            // xQueueSend(evt_queue, &evt_msg, portMAX_DELAY); // ส่งข้อมูลไป mqtt
        }
        else
        {
            if (press_state)
            {
                press_state = false;
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
