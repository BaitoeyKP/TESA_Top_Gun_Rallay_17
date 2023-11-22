#ifndef __MAIN_H__
#define __MAIN_H__

// include files
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FreeRTOS.h>
#include <Esp.h>
#include <esp_log.h>

// constants
#define TASK_BUTTON_PRIO 4
#define TASK_CAMERA_PRIO 3
#define TASK_MQTT_PRIO 2

#define TASK_BUTTON_TYPE 0
#define TASK_CAMERA_TYPE 1

#define WIFI_SSID "B"
#define WIFI_PASSWORD "0123456789"
#define MQTT_DEV_ID 22
#define MQTT_EVT_TOPIC "tgr2023/Luvinas/pub" // pub
#define MQTT_CMD_TOPIC "tgr2023/Luvinas/sub" // sub

#define EI_CAMERA_RAW_FRAME_BUFFER_COLS 240
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS 240
#define EI_CAMERA_FRAME_BYTE_SIZE 3
#define BMP_BUF_SIZE (EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE)

// type definitions typeหลักต้องtypeตามนี้
typedef struct evt_msg_t
{
    int type; // ต้องมี
    int id;
    uint32_t timestamp;
    bool pressed;
    uint32_t value;
} evt_msg_t;

// shared variables
extern xQueueHandle evt_queue;
extern bool enable_flag;
extern auto bb;

// public function prototypes

#endif