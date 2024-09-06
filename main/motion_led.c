#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Define the GPIO pins for the Red, Green, and Blue LEDs
#define GPIO_LED_RED 21
#define GPIO_LED_GREEN 19
#define GPIO_LED_BLUE 18

// Define the GPIO pin for the IR08H movement sensor
#define GPIO_SENSOR 27

// Define colors as RGB values in hexadecimal
#define RED     0xFF0000 // Red color
#define GREEN   0x00FF00 // Green color
#define BLUE    0x0000FF // Blue color
#define YELLOW  0xFFFF00 // Yellow color (combination of Red and Green)
#define VIOLET  0xEE82EE // Violet color

// Function to set RGB LED color
void set_rgb_color(uint32_t color) {
    // Extract individual RGB values from the 32-bit color value
    uint8_t red_value = (color >> 16) & 0xFF;
    uint8_t green_value = (color >> 8) & 0xFF;
    uint8_t blue_value = color & 0xFF;

    // Set the GPIO levels based on the color intensity
    gpio_set_level(GPIO_LED_RED, red_value > 0 ? 1 : 0);
    gpio_set_level(GPIO_LED_GREEN, green_value > 0 ? 1 : 0);
    gpio_set_level(GPIO_LED_BLUE, blue_value > 0 ? 1 : 0);
}

// Function to initialize the LEDs
void led_init() {
    gpio_set_direction(GPIO_LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_LED_BLUE, GPIO_MODE_OUTPUT);
}

// Function to initialize the movement sensor
void sensor_init() {
    gpio_set_direction(GPIO_SENSOR, GPIO_MODE_INPUT);
}

// Function to read the sensor value and change the LED color
void sensor_read() {
    int sensor_value = gpio_get_level(GPIO_SENSOR);
    if (sensor_value == 1) {
        // Sensor triggered, set LED color to YELLOW
        ESP_LOGI("SENSOR", "Motion detected! Changing color to YELLOW.");
        set_rgb_color(YELLOW);
    } else {
        // No motion, set LED color to BLUE
        ESP_LOGI("SENSOR", "No motion detected. Setting color to BLUE.");
        set_rgb_color(BLUE);
    }
}

void led_main() {
    // Initialize the LEDs and sensor
    led_init();
    sensor_init();

    while (1) {
        // Continuously check the sensor and change the LED color accordingly
        sensor_read();
        // Delay before reading sensor again (adjust as needed)
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second delay
    }
}
