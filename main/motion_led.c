#include "driver/ledc.h"

bool ledState = false;
unsigned long ledOnTime = 0;

void led_setup() {
    gpio_set_direction(23, GPIO_MODE_OUTPUT);
    gpio_set_direction(19, GPIO_MODE_OUTPUT);
    gpio_set_direction(18, GPIO_MODE_OUTPUT);
    }

void led_control(bool state) {
    if (state) {
        //analogWrite(23, 0);   // Red
        //analogWrite(19, 201); // Green
        //analogWrite(18, 204); // Blue
        //ledOnTime = millis();
        ledState = true;
    } else {
        //analogWrite(23, 0);
        //analogWrite(19, 0);
        //analogWrite(18, 0);s
        ledState = false;
    }
}

void check_led_timeout() {
    // if (ledState && (millis() - ledOnTime >= 3000)) {
    //     led_control(false);
    // }
    if (ledState) {
        led_control(false);
    }
}
