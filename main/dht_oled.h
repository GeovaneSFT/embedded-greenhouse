#include "ssd1306.h"


#ifndef DHT_OLED_H
#define DHT_OLED_H

void oled_main(float temperature, float humidity, float analogicTemp);
void read_dht(float *temperature, float *humidity);
void display_temperature_humidity(SSD1306_t *dev, float temperature, float humidity, float analogicTemp);

#endif // DHT_OLED_H
