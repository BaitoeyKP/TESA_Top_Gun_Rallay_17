#ifndef __TASK_CAMERA_H__
#define __TASK_CAMERA_H__

// include files
#include <Arduino.h>
#include <FreeRTOS.h>
#include <esp_log.h>
#include "main.h"
#include <Problem_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"

// shared variables

// public function prototypes
void task_camera_init(void);

#endif // __TASK_TASK_CAMERA_H__