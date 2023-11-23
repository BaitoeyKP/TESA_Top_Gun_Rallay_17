#include "main.h"
#include "net_mqtt.h"

// constants
#define TAG "main"
#define BTN_PIN 0
#define WIFI_SSID "TGR17_2.4G"
// #define WIFI_PASSWORD "0123456789"
#define MQTT_EVT_TOPIC "TGR_22/pub"
#define MQTT_CMD_TOPIC "TGR_22/#"
#define MQTT_DEV_ID 22

// static function prototypes
void print_memory(void);
static void mqtt_callback(char *topic, byte *payload, unsigned int length);

// static variables
static bool enable_flag = true;
// declare ArduinoJson objects for cmd_buf and evt_buf
StaticJsonDocument<128> cmd_buf;
StaticJsonDocument<128> evt_buf;

static char buf[128];

// Setup hardware
void setup()
{
  Serial.begin(115200);
  print_memory();
  pinMode(BTN_PIN, INPUT_PULLUP);
  // connect to WiFi
  // net_mqtt_init(WIFI_SSID, WIFI_PASSWORD);
  net_mqtt_init(WIFI_SSID);
  // connect to MQTT broker
  net_mqtt_connect(MQTT_DEV_ID, MQTT_CMD_TOPIC, mqtt_callback);
}

// Main loop
void loop()
{
  static uint32_t prev_millis = 0;

  // check button status
  if (digitalRead(BTN_PIN) == LOW)
  {
    // check button bounce
    if (millis() - prev_millis > 1000)
    {
      Serial.println("pressed");
      prev_millis = millis();
      // publish button event
      evt_buf["ID"] = MQTT_DEV_ID;
      evt_buf["timestamp"] = millis();
      evt_buf["pressed"] = true;
      serializeJson(evt_buf, buf);
      net_mqtt_publish(MQTT_EVT_TOPIC, buf);
    }
  }
  // loop MQTT interval
  net_mqtt_loop();
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