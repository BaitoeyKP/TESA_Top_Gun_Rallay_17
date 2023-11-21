#ifndef __MAIN_H__
#define __MAIN_H__

// include files
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FreeRTOS.h>
#include <Esp.h>
#include <esp_log.h>

// constants
#define TASK_BUTTON_PRIO        4
#define TASK_PERIOD_PRIO        3
#define TASK_MQTT_PRIO          2

#define TASK_BUTTON_TYPE        0
#define TASK_PERIOD_TYPE        1

#define WIFI_SSID       "B"
#define WIFI_PASSWORD   "0123456789"
#define MQTT_DEV_ID     220
#define MQTT_EVT_TOPIC  "tgr2023/Luvinas0/evt"
#define MQTT_CMD_TOPIC  "tgr2023/Luvinas0/cmd"

// type definitions
typedef struct evt_msg_t {
    int type;
    uint32_t timestamp;
    bool pressed;
    uint32_t value;
} evt_msg_t;

// shared variables
extern xQueueHandle evt_queue;
extern bool enable_flag;

// public function prototypes

#endif 