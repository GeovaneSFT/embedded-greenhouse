#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"  // Biblioteca para controle de LEDs com PWM
#include "esp_log.h"
#include "dht_oled.h"     // Inclusão para leitura de temperatura

// Define the GPIO pins for the Red, Green, and Blue LEDs
#define GPIO_LED_RED 21
#define GPIO_LED_GREEN 19
#define GPIO_LED_BLUE 18

// Define the GPIO pin for the PIR motion sensor
#define GPIO_PIR 27

typedef struct  {
    float temperature;
    float humidity;
} pir_data_t;

// Function to initialize the PIR motion sensor
void pir_init() {
    // Set the GPIO for the PIR sensor as an input
    gpio_reset_pin(GPIO_PIR);
    gpio_set_direction(GPIO_PIR, GPIO_MODE_INPUT);  // Define o pino como entrada
    ESP_LOGI("pir_INIT", "PIR sensor initialized on GPIO %d", GPIO_PIR);
}

// Function to set the RGB LED color using PWM
void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, red);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, green);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, blue);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
}

// Function to initialize PWM for RGB control
void ledc_init() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel[3] = {
        {
            .channel    = LEDC_CHANNEL_0,
            .duty       = 0,
            .gpio_num   = GPIO_LED_RED,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = LEDC_CHANNEL_1,
            .duty       = 0,
            .gpio_num   = GPIO_LED_GREEN,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = LEDC_CHANNEL_2,
            .duty       = 0,
            .gpio_num   = GPIO_LED_BLUE,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        }
    };

    for (int ch = 0; ch < 3; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }

    // Garantir que o LED inicie desligado
    set_rgb_color(0, 0, 0);
}

// Function to convert temperature to RGB values
void set_color_based_on_temperature(float temperature) {
    uint8_t red_value = 0, green_value = 0, blue_value = 0;

    if (temperature <= 25.0) {
        // Interpolação entre azul (0°C) e verde (25°C)
        green_value = (temperature / 25.0) * 255;
        blue_value = 255 - green_value;
    } else if (temperature > 25.0 && temperature <= 50.0) {
        // Interpolação entre verde (25°C) e vermelho (50°C)
        red_value = ((temperature - 25.0) / 25.0) * 255;
        green_value = 255 - red_value;
    } else if (temperature > 50.0) {
        // Piscando vermelho para temperaturas acima de 50°C
        for (int i = 0; i < 3; i++) {
            set_rgb_color(255, 0, 0);  // Vermelho
            vTaskDelay(pdMS_TO_TICKS(500));
            set_rgb_color(0, 0, 0);    // Desligado
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        return;
    }

    set_rgb_color(red_value, green_value, blue_value);
}

// Function to read the PIR sensor and control the LED based on motion and temperature
void pir_read(float temperature, float humidity) {

    int pir_value = gpio_get_level(GPIO_PIR);

    if (pir_value == 1) {
        ESP_LOGI("SENSOR", "Motion detected! Adjusting LED color based on temperature.");
        set_color_based_on_temperature(temperature);
    } else {
        ESP_LOGI("SENSOR", "No motion detected. Turning off LED.");
        set_rgb_color(0, 0, 0);  // Turn off LED
    }
}

// Main function to initialize components and run the LED control loop
void led_main(float temperature, float humidity) {
    ledc_init();     // Initialize PWM for LED control
    pir_init();   // Initialize PIR sensor

    while(1){
        read_dht(&temperature, &humidity);
        pir_read(temperature, humidity);
        vTaskDelay(1000);
    }
    vTaskDelete(NULL);  // Read sensor and control LED
    // vTaskDelay(pdMS_TO_TICKS(2000));  // Delay before next sensor read (2 second)
}    
