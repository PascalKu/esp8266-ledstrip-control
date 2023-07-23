#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FastLED.h>
#include "wifi_info.h"

#include <arduino_homekit_server.h>

#define FASTLED_ESP32_SPI_BUS HSPI
#define DATA_PIN    13
#define LED_RGB_SCALE 255       // this is the scaling factor used for color conversion
#define NUM_LEDS 5
CRGB leds[NUM_LEDS];

bool led_on = true;
float led_hue = 215;
float led_saturation = 59;      // saturation is scaled 0 to 100
float led_brightness = 100;     // brightness is scaled 0 to 100

void setup() {
  Serial.begin(115200);
  wifi_connect(); // in wifi_info.h

  printf("\n");
  printf("SketchSize: %d B\n", ESP.getSketchSize());
  printf("FreeSketchSpace: %d B\n", ESP.getFreeSketchSpace());
  printf("FlashChipSize: %d B\n", ESP.getFlashChipSize());
  printf("FlashChipRealSize: %d B\n", ESP.getFlashChipRealSize());
  printf("FlashChipSpeed: %d\n", ESP.getFlashChipSpeed());
  printf("SdkVersion: %s\n", ESP.getSdkVersion());
  printf("FullVersion: %s\n", ESP.getFullVersion().c_str());
  printf("CpuFreq: %dMHz\n", ESP.getCpuFreqMHz());
  printf("FreeHeap: %d B\n", ESP.getFreeHeap());
  printf("ResetInfo: %s\n", ESP.getResetInfo().c_str());
  printf("ResetReason: %s\n", ESP.getResetReason().c_str());
  
  DEBUG_HEAP();
  homekit_setup();
  DEBUG_HEAP();
  setup_led();
}

void loop() {
  homekit_loop();
  delay(5);
}

//==============================
// Homekit setup and loop
//==============================

extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t name;
extern "C" homekit_characteristic_t cha_on;
extern "C" homekit_characteristic_t cha_bright;
extern "C" homekit_characteristic_t cha_hue;
extern "C" homekit_characteristic_t cha_sat;

uint32_t next_heap_millis = 0;

void homekit_setup() {
  cha_on.setter = led_on_set;
  cha_on.getter = led_on_get;
  cha_bright.setter = led_bright_set;
  cha_sat.setter = led_sat_set;
  cha_sat.getter = led_sat_get;
  cha_hue.setter = led_hue_set;
  cha_hue.getter = led_hue_get;

  arduino_homekit_setup(&config);
  update_led();
}

void led_on_set(homekit_value_t value) {
  if (value.format != homekit_format_bool) {
    printf("ON: Invalid on-value format: %d\n", value.format);
    return;
  }

  led_on = value.bool_value;
  update_led();
}

homekit_value_t led_on_get() {
    return HOMEKIT_BOOL(led_on);
}

void led_bright_set(homekit_value_t value) {
  if (value.format != homekit_format_int) {
    printf("BRIGHT: Invalid on-value format: %d\n", value.format);
    return;
  }

  led_brightness = value.int_value;
  update_led();
}

homekit_value_t led_bright_get() {
    return HOMEKIT_INT(led_brightness);
}

void led_hue_set(homekit_value_t value) {
  if (value.format != homekit_format_float) {
    printf("HUE: Invalid on-value format: %d\n", value.format);
    return;
  }

  led_hue = value.float_value;
  update_led();
}

homekit_value_t led_hue_get() {
    return HOMEKIT_FLOAT(led_hue);
}

void led_sat_set(homekit_value_t value) {
  if (value.format != homekit_format_float) {
    printf("SAT: Invalid on-value format: %d\n", value.format);
    return;
  }
  led_saturation = value.float_value;
  update_led();
}

homekit_value_t led_sat_get() {
    return HOMEKIT_FLOAT(led_saturation);
}

void homekit_loop() {
  arduino_homekit_loop();
  uint32_t time = millis();
  if (time > next_heap_millis) {
    INFO("heap: %d, sockets: %d", ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
    next_heap_millis = time + 5000;
  }
}

void setup_led() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(100);
}

void update_led() {
  byte r = 0, g = 0, b = 0;

  if (led_on) {
    hsi2rgb(led_hue, led_saturation, led_brightness, &r, &g, &b);
  }

  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  FastLED.show();
  FastLED.show();
}
