#ifndef NVS_LOGGER_H
#define NVS_LOGGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Definir número máximo de logs e o tamanho da mensagem de log
#define MAX_LOG_ENTRIES 100
#define LOG_MESSAGE_MAX_LEN 128

// Estrutura de log
typedef struct {
    char message[LOG_MESSAGE_MAX_LEN];
    int error_code;
} log_entry_t;

// Funções públicas
void nvs_logger_init(void);
void enqueue_log(const char *message, int error_code);
void log_task(void *pvParameters);
void read_logs_from_nvs(void);

#endif // NVS_LOGGER_H
