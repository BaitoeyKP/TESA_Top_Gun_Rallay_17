#include "main.h"
#include "task_mqtt.h"
#include "net_mqtt.h"

// constants
#define TAG             "task_mqtt"

// static variables
StaticJsonDocument<128> evt_doc;

static char evt_buf[128];

// static function prototypes
static void task_mqtt_fcn(void* arg);

// task initialization
void task_mqtt_init(mqtt_callback_t mqtt_callback) {
    xTaskCreate(
        ,          /* Task function. */
        ,            /* String with name of task. */
        ,                   /* Stack size in bytes. */
        ,   /* Parameter passed as input of the task */
        ,         /* Priority of the task. */
        );                  /* Task handle. */
    ESP_LOGI(TAG, "task_mqtt created at %d", millis());
}

// task function
void task_mqtt_fcn(void* arg) {
    // task initialization
    net_mqtt_init(WIFI_SSID, WIFI_PASSWORD);
    net_mqtt_connect(MQTT_DEV_ID, MQTT_EVT_TOPIC, (mqtt_callback_t)arg);
    while(1) {
        static evt_msg_t evt_msg;
        // task function
        // wait for event

        ESP_LOGI(TAG, "task_mqtt run at %d", millis());
        evt_doc.clear();
        switch(evt_msg.type) {
            case TASK_BUTTON_TYPE:
                evt_doc["ID"] = MQTT_DEV_ID;
                evt_doc["timestamp"] = evt_msg.timestamp;
                evt_doc["pressed"] = evt_msg.pressed;
                serializeJson(evt_doc, evt_buf);
                break;
            case TASK_PERIOD_TYPE:
                evt_doc["ID"] = MQTT_DEV_ID;
                evt_doc["timestamp"] = evt_msg.timestamp;
                evt_doc["value"] = evt_msg.value;
                serializeJson(evt_doc, evt_buf);
                break;
            default:
                ESP_LOGW(TAG, "Unknown event type %d", evt_msg.type);
                evt_buf[0] = 0;
                break;
        }
        if (evt_buf[0] != 0) {
            net_mqtt_publish(MQTT_EVT_TOPIC, evt_buf);
        }
    }
}