#ifndef SLEEP_H
#define SLEEP_H

#include "driver/gpio.h"

// Define o GPIO do botão (botão de boot da ESP32)
#define BOTAO0 GPIO_NUM_0

// Funções públicas
void init_sleep_mode(void);
void check_button_and_sleep(void);

#endif // SLEEP_H
