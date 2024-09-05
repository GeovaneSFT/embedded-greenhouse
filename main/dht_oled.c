// dht_oled.c
#include <stdio.h>
#include <u8g2_esp32_hal.h>
#include <dht.h>
#include "dht_oled.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_log.h"
#include "mqtt_client.h"


#define DHT_GPIO 15       // GPIO for DHT11
#define I2C_SDA 21        // I2C SDA Pin
#define I2C_SCL 22        // I2C SCL Pin

static u8g2_t u8g2;

void init_oled() {
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.bus.i2c.sda = I2C_SDA;
    u8g2_esp32_hal.bus.i2c.scl = I2C_SCL;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_SetI2CAddress(&u8g2.u8x8, 0x78); // OLED address (shifted)
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
}

void read_dht(float *temperature, float *humidity) {
    if (dht_read_float_data(DHT_TYPE_DHT11, DHT_GPIO, humidity, temperature) != ESP_OK) {
        ESP_LOGE("DHT", "Failed to read data from DHT sensor");
    }
}

void display_oled(float temperature, float humidity) {
    u8g2_ClearBuffer(&u8g2);
    
    char temp_str[16];
    char hum_str[16];

    sprintf(temp_str, "Temp: %.1f C", temperature);
    sprintf(hum_str, "Humidity: %.1f %%", humidity);

    u8g2_DrawStr(&u8g2, 0, 20, temp_str);
    u8g2_DrawStr(&u8g2, 0, 40, hum_str);

    u8g2_SendBuffer(&u8g2);
}
