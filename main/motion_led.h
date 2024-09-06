#ifndef MOTION_LED_H
#define MOTION_LED_H

#include <stdbool.h>

// Definir constantes para os LEDs
#define LEDC_CHANNEL_0    LEDC_CHANNEL_0
#define LEDC_CHANNEL_1    LEDC_CHANNEL_1
#define LEDC_CHANNEL_2    LEDC_CHANNEL_2
#define LEDC_TIMER        LEDC_TIMER_0
#define LEDC_MODE         LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO_R  (23) // GPIO para o LED vermelho
#define LEDC_OUTPUT_IO_G  (19) // GPIO para o LED verde
#define LEDC_OUTPUT_IO_B  (18) // GPIO para o LED azul
#define LEDC_DUTY_RES     LEDC_TIMER_8_BIT // Resolução de 8 bits
#define LEDC_FREQUENCY    (5000) // Frequência de 5kHz

// Variáveis globais
extern bool ledState;
extern unsigned long ledOnTime;

// Funções de controle do LED
void ledc_init(void);
void led_control(bool state);
void check_led_timeout(void);
void sensor_init(void);
void led_main(void);

#endif // MOTION_LED_H
