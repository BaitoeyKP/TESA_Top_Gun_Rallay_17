#include "main.h"
#include "hw_camera.h"
// add header file of Edge Impulse firmware

#include <Problem_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"

// constants
#define TAG "main"

#define BTN_PIN 0

#define EI_CAMERA_RAW_FRAME_BUFFER_COLS 240
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS 240
#define EI_CAMERA_FRAME_BYTE_SIZE 3
#define BMP_BUF_SIZE (EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE)

// static variables
static uint8_t *bmp_buf;

// static int save_ = 0;
// unsigned int y1 = 0;
// unsigned int y2 = 0;
// static unsigned int ubu = 0;
// static unsigned int spon = 0;
// static char gg[10];

// static function prototypes
void print_memory(void);
void ei_prepare_feature(uint8_t *img_buf, signal_t *signal);
int ei_get_feature_callback(size_t offset, size_t length, float *out_ptr);
int ei_use_result(ei_impulse_result_t result);

// initialize hardware
void setup()
{
  Serial.begin(115200);
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
}

// main loop
void loop()
{
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
      ei_use_result(result);
      // use result
      // if (ei_use_result(result) == 1)
      // {
      //   save_ += 1;
      // }

      press_state = true;
    }
  }
  else
  {
    if (press_state)
    {
      press_state = false;
    }
  }

  // if (save_ == 2)
  // {
  //   if (ubu > spon && (!(ubu != 555 && spon == 555) && !(ubu == 555 && spon != 555) && !(ubu == 555 && spon == 555)))
  //   {
  //     ESP_LOGI(TAG, "S อยู่ข้างบน U");
  //   }
  //   else if (spon > ubu && (!(ubu != 555 && spon == 555) && !(ubu == 555 && spon != 555) && !(ubu == 555 && spon == 555)))
  //   {
  //     ESP_LOGI(TAG, "U อยู่ข้างบน S");
  //   }
  //   ubu = 555;
  //   spon = 555;
  //   save_ = 0;
  // }

  delay(100);
}

// Print memory information
void print_memory()
{
  ESP_LOGI(TAG, "Total heap: %u", ESP.getHeapSize());
  ESP_LOGI(TAG, "Free heap: %u", ESP.getFreeHeap());
  ESP_LOGI(TAG, "Total PSRAM: %u", ESP.getPsramSize());
  ESP_LOGI(TAG, "Free PSRAM: %d", ESP.getFreePsram());
}

// prepare feature
void ei_prepare_feature(uint8_t *img_buf, signal_t *signal)
{
  signal->total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
  signal->get_data = &ei_get_feature_callback;
  if ((EI_CAMERA_RAW_FRAME_BUFFER_ROWS != EI_CLASSIFIER_INPUT_WIDTH) || (EI_CAMERA_RAW_FRAME_BUFFER_COLS != EI_CLASSIFIER_INPUT_HEIGHT))
  {
    ei::image::processing::crop_and_interpolate_rgb888(
        img_buf,
        EI_CAMERA_RAW_FRAME_BUFFER_COLS,
        EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        img_buf,
        EI_CLASSIFIER_INPUT_WIDTH,
        EI_CLASSIFIER_INPUT_HEIGHT);
  }
}

// get feature callback
int ei_get_feature_callback(size_t offset, size_t length, float *out_ptr)
{
  size_t pixel_ix = offset * 3;
  size_t pixels_left = length;
  size_t out_ptr_ix = 0;

  while (pixels_left != 0)
  {
    out_ptr[out_ptr_ix] = (bmp_buf[pixel_ix] << 16) + (bmp_buf[pixel_ix + 1] << 8) + bmp_buf[pixel_ix + 2];

    // go to the next pixel
    out_ptr_ix++;
    pixel_ix += 3;
    pixels_left--;
  }
  return 0;
}

// use result from classifier
int ei_use_result(ei_impulse_result_t result)
{
  ESP_LOGI(TAG, "Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
           result.timing.dsp, result.timing.classification, result.timing.anomaly);
  bool bb_found = result.bounding_boxes[0].value > 0;
  for (size_t ix = 0; ix < result.bounding_boxes_count; ix++)
  {
    auto bb = result.bounding_boxes[ix];
    if (bb.value == 0)
    {
      continue;
    }
    ESP_LOGI(TAG, "%s (%f) [ x: %u, y: %u, width: %u, height: %u ]", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);

    // printf("1: %u", bb.y);

    // if (bb.label == "ubu")
    // {
    //   ubu = bb.y;
    // }
    // if (bb.label == "spon")
    // {
    //   spon = bb.y;
    // }
  }
  if (!bb_found)
  {
    ESP_LOGI(TAG, "No objects found");
    return 0;
  }
  return 1;
}