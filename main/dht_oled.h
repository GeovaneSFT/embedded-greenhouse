#ifndef DHT_OLED_H
#define DHT_OLED_H

void init_oled();
void read_dht(float *temperature, float *humidity);
void display_oled(float temperature, float humidity);

#endif // DHT_OLED_H
