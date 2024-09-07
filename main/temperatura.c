#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

#define TAG "THERMISTOR"

// Define as constantes para o termistor NTC do sensor KY-013
#define SERIES_RESISTOR 10000.0 // 10kΩ resistor em série
#define B_COEFFICIENT 3950.0	// Coeficiente B do termistor
#define ADC_MAX 4095			// Valor máximo do ADC (12 bits)
#define SUPPLY_VOLTAGE 3.3		// Tensão de alimentação da ESP32

int sensorPin = 34;
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;

void init_adc()
{
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
}

double Thermistor(int Vo)
{
 R2 = R1 * Vo/(1023 - Vo);
 logR2 = log(R2);
 T = 1.0 /(c1 + c2*logR2 + c3*logR2*logR2*logR2);
 T = T - 273.15; // convert from Kelvin to Celsius
 return T;
}

int Vo;


float thermistor_main(float analogicTemp)
{
	init_adc();

    Vo = adc1_get_raw(ADC1_CHANNEL_6);
    ESP_LOGI(TAG, "Valor do ADC: %d", Vo); // Imprime o valor do ADC

    analogicTemp = Thermistor(Vo);
    if (isnan(analogicTemp))
    {
        ESP_LOGI(TAG, "Leitura inválida do ADC.");
    }
    else
    {
        ESP_LOGI(TAG, "Temperatura ANALOGICA: %.2f ºC", analogicTemp);
    }

    return analogicTemp;

}