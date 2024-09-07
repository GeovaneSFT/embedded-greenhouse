#include "nvs_logger.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include <string.h>
#include <inttypes.h> // For PRIu32

static const char *TAG = "NVS_LOGGER";
static QueueHandle_t log_queue;  // Fila de logs

// Função para inicializar o NVS e a fila de logs
void nvs_logger_init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Criar fila de logs
    log_queue = xQueueCreate(MAX_LOG_ENTRIES, sizeof(log_entry_t));
    if (log_queue == NULL) {
        ESP_LOGE(TAG, "Falha ao criar a fila de logs");
    }

    // Criar a task de gerenciamento de logs
    xTaskCreate(log_task, "log_task", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "NVS inicializado e task de logs iniciada.");
}

// Função para enfileirar logs
void enqueue_log(const char *message, int error_code) {
    if (log_queue == NULL) {
        ESP_LOGE(TAG, "Fila de logs não inicializada");
        return;
    }

    log_entry_t log_entry;
    snprintf(log_entry.message, LOG_MESSAGE_MAX_LEN, "%s", message);
    log_entry.error_code = error_code;

    // Enfileirar o log
    if (xQueueSend(log_queue, &log_entry, pdMS_TO_TICKS(10)) != pdPASS) {
        ESP_LOGW(TAG, "Fila de logs cheia, log descartado");
    }
}

// Função de gerenciamento de logs que roda como uma task
void log_task(void *pvParameters) {
    nvs_handle_t my_handle;
    esp_err_t err;

    // Abrir o NVS em modo leitura/escrita
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao abrir o NVS: %s", esp_err_to_name(err));
        vTaskDelete(NULL); // Encerrar a task se não for possível abrir o NVS
    }

    uint32_t log_head = 0, log_count = 0;
    uint32_t log_tail = 0;  // Changed to uint32_t

    // Recuperar o estado atual da fila circular
    nvs_get_u32(my_handle, "log_head", &log_head);
    nvs_get_u32(my_handle, "log_tail", &log_tail);
    nvs_get_u32(my_handle, "log_count", &log_count);

    log_entry_t log_entry;

    while (1) {
        // Esperar um novo log na fila (tempo infinito)
        if (xQueueReceive(log_queue, &log_entry, portMAX_DELAY)) {
            // Criar chave de log com base na posição atual do `log_tail`
            char log_key[16];
            snprintf(log_key, sizeof(log_key), "log_%" PRIu32, log_tail);

            // Formatar a mensagem de log
            char log_message[LOG_MESSAGE_MAX_LEN + 32];
            snprintf(log_message, sizeof(log_message), "Error: %s, Code: 0x%x", log_entry.message, log_entry.error_code);

            // Gravar o log no NVS na posição `log_tail`
            err = nvs_set_str(my_handle, log_key, log_message);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Erro ao gravar o log no NVS: %s", esp_err_to_name(err));
            }

            // Incrementar `log_tail` e garantir que seja circular
            log_tail = (log_tail + 1) % MAX_LOG_ENTRIES;

            // Se o `log_tail` alcançar o `log_head`, o log mais antigo será sobrescrito
            if (log_count == MAX_LOG_ENTRIES) {
                log_head = (log_head + 1) % MAX_LOG_ENTRIES;
            } else {
                log_count++;
            }

            // Atualizar o estado da fila circular no NVS
            nvs_set_u32(my_handle, "log_head", log_head);
            nvs_set_u32(my_handle, "log_tail", log_tail);
            nvs_set_u32(my_handle, "log_count", log_count);

            // Confirmar gravação no NVS
            nvs_commit(my_handle);
        }

        // Delay opcional para evitar gravações muito frequentes
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Fechar o NVS (nunca deve chegar aqui devido ao loop infinito)
    nvs_close(my_handle);
}

// Função para ler todos os logs na ordem correta (FIFO)
void read_logs_from_nvs(void) {
    nvs_handle_t my_handle;
    esp_err_t err;

    // Abrir NVS em modo leitura
    err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao abrir o NVS: %s", esp_err_to_name(err));
        return;
    }

    uint32_t log_head = 0, log_count = 0;
    uint32_t log_tail = 0;  // Changed to uint32_t

    // Recuperar o estado da fila circular
    nvs_get_u32(my_handle, "log_head", &log_head);
    nvs_get_u32(my_handle, "log_tail", &log_tail);
    nvs_get_u32(my_handle, "log_count", &log_count);

    // Ler os logs na ordem correta, começando pelo `log_head`
    for (uint32_t i = 0; i < log_count; i++) {
        char log_key[16];
        snprintf(log_key, sizeof(log_key), "%" PRIu32, (log_head + i) % MAX_LOG_ENTRIES);

        char log_entry[LOG_MESSAGE_MAX_LEN + 32];
        size_t log_entry_size = sizeof(log_entry);
        err = nvs_get_str(my_handle, log_key, log_entry, &log_entry_size);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Log %" PRIu32 ": %s", i, log_entry);
        } else {
            ESP_LOGE(TAG, "Erro ao ler o log %" PRIu32 ": %s", i, esp_err_to_name(err));
        }
    }

    // Fechar o NVS
    nvs_close(my_handle);
}
