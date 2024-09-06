#ifndef TEMPERATURA_H
#define TEMPERATURA_H

#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

// Definindo as constantes para o sensor KY-013
#define SERIES_RESISTOR 10000.0 // 10kΩ resistor em série
#define B_COEFFICIENT 3950.0    // Coeficiente B do termistor
#define ADC_MAX 4095            // Valor máximo do ADC (12 bits)
#define SUPPLY_VOLTAGE 3.3      // Tensão de alimentação da ESP32

// Função para inicializar o ADC
void init_adc(void);

// Função para calcular a temperatura a partir do valor do ADC
double Thermistor(int RawADC);

// Função principal para leitura do termistor e cálculo da temperatura
float thermistor_main(float analogicTemp);

#endif // TEMPERATURA_H
