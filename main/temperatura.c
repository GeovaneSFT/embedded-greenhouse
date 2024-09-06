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

void init_adc()
{
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
}

double Thermistor(int RawADC)
{
	// Verifica se o valor do ADC é válido
	if (RawADC <= 0 || RawADC >= ADC_MAX)
	{
		return NAN; // Retorna NaN se o valor do ADC for inválido
	}

	// Converte o valor ADC para uma tensão
	double voltagem = (RawADC * SUPPLY_VOLTAGE) / ADC_MAX;

	// Calcula a resistência do termistor com base na tensão lida
	double resistencia = SERIES_RESISTOR * (SUPPLY_VOLTAGE / voltagem - 1.0);

	// Verifica se a resistência calculada é razoável
	if (resistencia <= 0.0)
	{
		return NAN; // Retorna NaN se a resistência calculada for inválida
	}

	// Aplica a equação de Steinhart-Hart para calcular a temperatura
	double steinhart;
	steinhart = resistencia / SERIES_RESISTOR; // (R/Ro)
	steinhart = log(steinhart);				   // ln(R/Ro)
	steinhart /= B_COEFFICIENT;				   // 1/B * ln(R/Ro)
	steinhart += 1.0 / (25.0 + 273.15);		   // + (1/To), onde To é 25ºC em Kelvin
	steinhart = 1.0 / steinhart;			   // Inverte
	steinhart -= 273.15;					   // Converte para Celsius

	return steinhart;
}

float thermistor_main(float analogicTemp)
{
	init_adc();

    int adcValue = adc1_get_raw(ADC1_CHANNEL_6);
    ESP_LOGI(TAG, "Valor do ADC: %d", adcValue); // Imprime o valor do ADC

    analogicTemp = Thermistor(adcValue);
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