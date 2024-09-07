#include <stdio.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sleep.h"

static const char *TAG = "SLEEP_MODE";
#define BOTAO0 GPIO_NUM_0

// Função para inicializar o modo de sleep
void init_sleep_mode(void) {
    // Configurando GPIO0 (botão de boot) como entrada
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;  // Desabilita interrupções
    io_conf.mode = GPIO_MODE_INPUT;         // Configura como entrada
    io_conf.pin_bit_mask = (1ULL << BOTAO0); // Seleciona o GPIO0
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE; // Habilita pull-down interno
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;    // Desabilita pull-up

    gpio_config(&io_conf);  // Configura o GPIO

    // Verificar se estamos acordando de deep sleep
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO) {
        ESP_LOGI(TAG, "Acordou do deep sleep pelo GPIO0 (botão de boot)");
    } else {
        ESP_LOGI(TAG, "Iniciando normalmente (não acordou de deep sleep)");
    }

    // Mensagem inicial
    ESP_LOGI(TAG, "Pressione o botão GPIO0 para entrar no modo Deep Sleep...\n");
}

// Função para verificar o botão e entrar em deep sleep se pressionado
void check_button_and_sleep(void) {
        if (gpio_get_level(BOTAO0) == 0) // Verifica se o botão foi pressionado (nível baixo)
        {
            ESP_LOGI(TAG, "Botão pressionado, entrando em Deep Sleep...");
            vTaskDelay(pdMS_TO_TICKS(1000));  // Delay para evitar problemas de bouncing do botão

            // Configura o GPIO0 como fonte de wakeup
            esp_sleep_enable_ext0_wakeup(BOTAO0, 0); // Acorda quando o botão for pressionado (nível baixo)

            // Entrar no modo deep sleep
            esp_deep_sleep_start();
        }

        // Delay para evitar sobrecarga de CPU
        vTaskDelay(pdMS_TO_TICKS(200));
}
