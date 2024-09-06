#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "dht.h"  // Usando a biblioteca dht.h
#include "driver/gpio.h"

#define TAG "SSD1306"
#define DHT_GPIO 25   // GPIO usado para o DHT11

// Função para ler os dados do DHT11
void read_dht(float *temperature, float *humidity) {
    dht_read_float_data(DHT_TYPE_DHT11, DHT_GPIO, humidity, temperature);
}

// Função para exibir temperatura e umidade no display OLED
void display_temperature_humidity(SSD1306_t *dev, float temperature, float humidity, float analogicTemp) {
    char buffer[20];
    
    ssd1306_clear_screen(dev, false);

    // Exibir a temperatura
    sprintf(buffer, "Temp: %.1fC", temperature);
    ssd1306_display_text(dev, 0, buffer, strlen(buffer), false);

    // Exibir a umidade
    sprintf(buffer, "Humid: %.1f%%", humidity);
    ssd1306_display_text(dev, 1, buffer, strlen(buffer), false);

    sprintf(buffer, "Analogic: %.1fC", analogicTemp);
    ssd1306_display_text(dev, 2, buffer, strlen(buffer), false);
}

void oled_main(float temperature, float humidity, float analogicTemp)
{
    // Inicializando o display OLED
    SSD1306_t dev;
    ESP_LOGI(TAG, "Initializing OLED...");

#if CONFIG_I2C_INTERFACE
    ESP_LOGI(TAG, "Using I2C interface");
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

#if CONFIG_SPI_INTERFACE
    ESP_LOGI(TAG, "Using SPI interface");
    spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_SPI_INTERFACE

#if CONFIG_SSD1306_128x64
    ssd1306_init(&dev, 128, 64);
#endif
#if CONFIG_SSD1306_128x32
    ssd1306_init(&dev, 128, 32);
#endif

    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xff);
        // Exibir temperatura e umidade no display
    display_temperature_humidity(&dev, temperature, humidity, analogicTemp);

        // Aguardar 2 segundos antes de ler novamente
}
