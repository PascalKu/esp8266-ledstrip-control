#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void custom_accessory_identify(homekit_value_t _value) {
  printf("accessory identify\n");
}

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "LEDPlant");
homekit_characteristic_t cha_name = HOMEKIT_CHARACTERISTIC_(NAME, "LEDPlant");
homekit_characteristic_t cha_on = HOMEKIT_CHARACTERISTIC_(ON, false);
homekit_characteristic_t cha_bright = HOMEKIT_CHARACTERISTIC_(BRIGHTNESS, 100);
homekit_characteristic_t cha_hue = HOMEKIT_CHARACTERISTIC_(HUE, (float) 215);
homekit_characteristic_t cha_sat = HOMEKIT_CHARACTERISTIC_(SATURATION, (float) 59);

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_lightbulb, .services = (homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics = (homekit_characteristic_t*[]) {
            &name,
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Pascal Kutscha"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456"),
            HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266/LEDPlant"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, custom_accessory_identify),
            NULL
        }),
        HOMEKIT_SERVICE(LIGHTBULB, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
            &cha_name,
            &cha_on,
            &cha_bright,
            &cha_hue,
            &cha_sat,
            NULL
        }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
    .accessories = accessories,
    .password = "111-11-111"
};
