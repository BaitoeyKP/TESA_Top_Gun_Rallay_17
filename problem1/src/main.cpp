#include "main.h"
#include "hw_mic.h"
#include "net_mqtt.h"

#define TAG "main"
#define BTN_PIN 0
#define WIFI_SSID "B"
#define WIFI_PASSWORD "0123456789"
#define MQTT_EVT_TOPIC "tgr2023/Luvinas/btn_evt"
#define MQTT_CMD_TOPIC "tgr2023/Luvinas/cmd"
#define MQTT_DEV_ID 22

#define SAMPLE_RATE 16000
#define NUM_SAMPLES 160
static unsigned int num_samples = NUM_SAMPLES;
#define SCALE 1e6
static int32_t samples[160];

// static function prototypes
void print_memory(void);
static void mqtt_callback(char *topic, byte *payload, unsigned int length);

// static variables
static bool enable_flag = true;
// declare ArduinoJson objects for cmd_buf and evt_buf
StaticJsonDocument<128> cmd_buf;
StaticJsonDocument<128> evt_buf;

static char buf[128];

const float VOICE_THRESHOLD = 2;
bool isListening = false;
uint8_t period = 0;
float avg = 0;

void setup()
{
  Serial.begin(115200);
  hw_mic_init(SAMPLE_RATE);
  print_memory();
  pinMode(BTN_PIN, INPUT_PULLUP);
  // connect to WiFi
  net_mqtt_init(WIFI_SSID, WIFI_PASSWORD);
  // connect to MQTT broker
  net_mqtt_connect(MQTT_DEV_ID, MQTT_CMD_TOPIC, mqtt_callback);
}

void loop()
{
  // ESP_LOGI(TAG, "loop at %d", millis());
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
    period = millis();
    isListening = true;
  }
  else if (isListening && avg <= VOICE_THRESHOLD)
  {
    Serial.println("End of AHHHH detected!");
    Serial.println(millis() - period);
    isListening = false;
  }
  static uint32_t prev_millis = 0;

  if (enable_flag)
  {
    // check button status
    if (digitalRead(BTN_PIN) == LOW)
    {
      // check button bounce
      if (millis() - prev_millis > 1000)
      {
        prev_millis = millis();
        // publish button event
        evt_buf["ID"] = MQTT_DEV_ID;
        evt_buf["timestamp"] = millis();
        evt_buf["sound"] = isListening;
        evt_buf["duration"] = period;
        serializeJson(evt_buf, buf);
        net_mqtt_publish(MQTT_EVT_TOPIC, buf);
      }
    }
    // loop MQTT interval
    net_mqtt_loop();
    delay(10);
  }
}

// Print memory information
void print_memory()
{
  ESP_LOGI(TAG, "Total heap: %u", ESP.getHeapSize());
  ESP_LOGI(TAG, "Free heap: %u", ESP.getFreeHeap());
  ESP_LOGI(TAG, "Total PSRAM: %u", ESP.getPsramSize());
  ESP_LOGI(TAG, "Free PSRAM: %d", ESP.getFreePsram());
}

// callback function to handle MQTT message
void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
  ESP_LOGI(TAG, "Message arrived on topic %s", topic);
  ESP_LOGI(TAG, "Payload: %.*s", length, payload);
  // extract data from payload
  char tmpbuf[128];
  memcpy(tmpbuf, payload, length);
  tmpbuf[length] = 0;
  // check if the message is for this device
  deserializeJson(cmd_buf, tmpbuf);
  // configure enable/disable status
  if (cmd_buf["ID"] == MQTT_DEV_ID)
  {
    enable_flag = cmd_buf["enable"];
  }
}