#include "main.h"
#include "task_period.h"
#include "hw_mic.h"
#include "net_mqtt.h"

// constants
#define TAG "task_period"

#define BTN_PIN 0

#define SAMPLE_RATE 16000
#define NUM_SAMPLES 160
static unsigned int num_samples = NUM_SAMPLES;
#define SCALE 1e6
static int32_t samples[160];
const float VOICE_THRESHOLD = 2;
bool isListening = false;
uint8_t period = 0;
float avg = 0;

// static function prototypes
static void task_period_fcn(void *arg);

// task initialization
void task_period_init(uint32_t period_ms)
{
    static uint32_t arg = period_ms;
    xTaskCreate(
        task_period_fcn,  /* Task function. */
        "Periodic Task",  /* String with name of task. */
        2048,             /* Stack size in bytes. */
        &arg,             /* Parameter passed as input of the task */
        TASK_PERIOD_PRIO, /* Priority of the task. */
        NULL);            /* Task handle. */
    ESP_LOGI(TAG, "task_period created at %d", millis());
}

// task function
void task_period_fcn(void *arg)
{
    // task initialization
    uint32_t period_ms = *((uint32_t *)arg);
    while (1)
    {
        // task function
        evt_msg_t evt_msg = {
            .type = TASK_PERIOD_TYPE,
            .id = MQTT_DEV_ID,
            .timestamp = millis(),
            .pressed = false,
            .value = 0};
        num_samples = NUM_SAMPLES;
        hw_mic_read(samples, &num_samples);
        float sample_avg = 0;
        for (int i = 0; i < num_samples; i++)
        {
            sample_avg += (float)samples[i] / SCALE;
        }
        float avg_normalized = sample_avg / num_samples;
        avg = abs(avg_normalized);
        Serial.println(avg);
        if (!isListening && avg > VOICE_THRESHOLD)
        {
            Serial.println("Start of AHHHH detected!");
            evt_msg.value = 0;
            period = millis();
            isListening = true;
            evt_msg.pressed = true;
        }
        else if (isListening && avg <= VOICE_THRESHOLD)
        {
            Serial.println("End of AHHHH detected!");
            Serial.println(millis() - period);
            evt_msg.value = millis() - period;
            isListening = false;
            evt_msg.pressed = false;
        }
        // ESP_LOGI(TAG, "task_period run at %d", millis());
        xQueueSend(evt_queue, &evt_msg, portMAX_DELAY);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
