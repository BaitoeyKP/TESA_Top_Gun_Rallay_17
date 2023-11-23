#ifndef __MAIN_H__
#define __MAIN_H__

// include files
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FreeRTOS.h>
#include <Esp.h>
#include <esp_log.h>

// constants
#define TASK_BUTTON_PRIO 3
#define TASK_CAMERA_PRIO 2
#define TASK_MQTT_PRIO 4

#define TASK_BUTTON_TYPE 0
#define TASK_CAMERA_TYPE 1

#define WIFI_SSID "B"
#define WIFI_PASSWORD "0123456789"
#define MQTT_DEV_ID 22
#define MQTT_EVT_TOPIC "tgr2023/Luvinas/pub"
#define MQTT_CMD_TOPIC "tgr2023/Luvinas/sub"

// type definitions
typedef struct evt_msg_t
{
    int type;
    uint32_t timestamp;
    bool pressed;
    uint32_t value;
    int label;
} evt_msg_t;

// shared variables
extern xQueueHandle evt_queue;
extern bool enable_flag;
extern int label;

// public function prototypes

#endif