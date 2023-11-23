#ifndef __DETECTION_H__
#define __DETECTION_H__

// include files
#include <Arduino.h>
#include <esp_log.h>
#include <Problem_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"


// shared variables
static uint8_t *bmp_buf;

// public function prototypes
void print_memory();
void ei_prepare_feature(uint8_t *img_buf, signal_t *signal);
int ei_get_feature_callback(size_t offset, size_t length, float *out_ptr);
int ei_use_result(ei_impulse_result_t result);

#endif // __DETECTION_H__