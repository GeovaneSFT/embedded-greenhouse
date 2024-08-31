#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"


#define TAG "THERMISTOR"

int sensorPin = 36


void init_adc() {
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
}

double Thermister(int RawADC){
	double Temp;
	Temp = log(((10240000/RawADC) - 10000));
	Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp)) * Temp);
	Temp = Temp - 273.15;
	return Temp;
}

/* void setup() {
	// codigo setup
	Serial.begin(9600);
}
*/

void app_main() {
	init_adc();
	
	while(1) {
		int adcValue = adc1_get_raw(ADC1_CHANNEL_0);

		double temperature = Thermister(adcValue);
		ESP_LOGI(TAG, "Temperatura: %.2f ºC", temperature);

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
