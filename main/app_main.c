/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* MQTT (over TCP) Example with custom outbox

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "mqtt_client.h"
#include "esp_log.h"
#include "nvs_logger.h"

#include "dht_oled.h"
#include "dht.h"
#include "ssd1306.h"
#include "driver/i2c.h"
#include "motion_led.h"
#include "temperatura.h"
#include "wifi.h"
#include "sleep.h"

#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

static const char *TAG = "MQTT_EXAMPLE";
static const char *TAG_BROKER = "MQTT_SUBSCRIBER";

float minHumidity = 0.0;
float minTemperature = 0.0;
float maxHumidity = 0.0;
float maxTemperature = 0.0;
float analogicTemp = 0;

esp_mqtt_client_handle_t client;
esp_mqtt_client_handle_t mosquitto;

typedef struct pir_data_t {
    float temperature;
    float humidity;
} pir_data_t;

struct pir_data_t pir = {
    .temperature = 0,
    .humidity = 0
};

int prev_pir_value = 0;

// Define as constantes para o termistor NTC do sensor KY-013
#define SERIES_RESISTOR 10000.0 // 10kΩ resistor em série
#define B_COEFFICIENT 3950.0	// Coeficiente B do termistor
#define ADC_MAX 4095			// Valor máximo do ADC (12 bits)
#define SUPPLY_VOLTAGE 3.3		// Tensão de alimentação da ESP32

void pir_led_task(void *pvParameters) {
    pir_data_t *pir = (pir_data_t *)pvParameters;  // Conversão do ponteiro para estrutura

    ledc_init();
    pir_init();

    while(1) {
        // Leitura do PIR com base nos dados de temperatura e umidade
        pir_read(pir->temperature, pir->humidity);  // Controle do LED baseado no PIR e temperatura
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 3 segundos
    }

    vTaskDelete(NULL);  // Deleta a task (embora aqui seja um loop infinito)
}

static void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
        enqueue_log(message, error_code);  // Enfileirar o log
    }
}

void dht_task(void *pvParameters) {
    pir_data_t *pir = (pir_data_t *)pvParameters;  // Conversão do ponteiro para estrutura

    while(1) {
        // Leitura do DHT11 com base nos dados de temperatura e umidade
        read_dht(&pir->temperature, &pir->humidity);
        oled_main(pir->temperature, pir->humidity, analogicTemp);  // Controle do LED baseado no DHT11 e temperatura
        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay de 1 segundo
    }

    vTaskDelete(NULL);  // Deleta a task (embora aqui seja um loop infinito)
}

void thermistor_task(void *pvParameters) {
    while (1) {
        analogicTemp = thermistor_main(analogicTemp);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);  // Deleta a task (embora aqui seja um loop infinito)
}



/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_publish(client, "v1/devices/me/telemetry", "{\"temperatura\":25,\"humidity\":50}", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "verificador-umidade/sensores/placa1", 1);
        ESP_LOGI(TAG_BROKER, "Subscribed to verificador_umidade/sensores/placa1");


        // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);



        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        // ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        // msg_id = esp_mqtt_client_subscribe(client, "umidade/sensores/placa1", 1);
        // ESP_LOGI(TAG_BROKER, "Subscribed to umidade/sensores/placa1");

        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG_BROKER, "RECEIVED MESSAGE: TOPIC: %.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG_BROKER, "RECEIVED MESSAGE: DATA: %.*s", event->data_len, event->data);

        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG_BROKER, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG_BROKER, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void message_receiver(esp_mqtt_event_handle_t receive) {
    ESP_LOGI(TAG, "RECEIVED MESSAGE: TOPIC: %.*s", receive->topic_len, receive->topic);
    ESP_LOGI(TAG, "RECEIVED MESSAGE: DATA: %.*s", receive->data_len, receive->data);

    // Parse the JSON manually (for simplicity, assuming the message format is correct)
    if (strstr(receive->data, "\"minTemperatura\":") && strstr(receive->data, "\"minUmidade\":")) {
        char *minTemp_ptr = strstr(receive->data, "\"minTemperatura\":");
        char *minHumidity_ptr = strstr(receive->data, "\"minUmidade\":");

        if (minTemp_ptr && minHumidity_ptr) {
            sscanf(minTemp_ptr, "\"minTemperatura\":%f", &minTemperature);
            sscanf(minHumidity_ptr, "\"minUmidade\":%f", &minHumidity);

            ESP_LOGI(TAG, "Parsed minTemperature: %.1f, minHumidity: %.1f", minTemperature, minHumidity);
        } else {
            ESP_LOGE(TAG, "Failed to find minTemperatura or minUmidade in JSON");
        }
    }
}



static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://164.41.98.25",
        .credentials.username = "0bBzQsrYbDDW22GT7Fr1"};

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    esp_mqtt_client_config_t mqtt_mosquitto = {
        .broker.address.uri = "mqtt://test.mosquitto.org",
    };

    mosquitto = esp_mqtt_client_init(&mqtt_mosquitto);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mosquitto);

#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.broker.address.uri, "FROM_STDIN") == 0)
    {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128)
        {
            int c = fgetc(stdin);
            if (c == '\n')
            {
                line[count] = '\0';
                break;
            }
            else if (c > 0 && c < 127)
            {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.broker.address.uri = line;
        printf("Broker url: %s\n", line);
    }
    else
    {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    /*Let's enqueue a few messages to the outbox to see the allocations*/
    int msg_id;
    msg_id = esp_mqtt_client_enqueue(client, "/topic/qos1", "data_3", 0, 1, 0, true);
    ESP_LOGI(TAG, "Enqueued msg_id=%d", msg_id);
    msg_id = esp_mqtt_client_enqueue(client, "/topic/qos2", "QoS2 message", 0, 2, 0, true);
    ESP_LOGI(TAG, "Enqueued msg_id=%d", msg_id);

    /* Now we start the client and it's possible to see the memory usage for the operations in the outbox. */
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("custom_outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    // wifi_start()// ;

    mqtt_app_start();
    // init_oled();

    init_sleep_mode();

    pir.temperature = 0.0;
    pir.humidity = 0.0;

    float maxTemperature = 0.0;
//     float maxHumidity = 0.0;
//     float analogicTemp = 0.0;

    //Task do PIR e LED
    oled_init();
    xTaskCreate(pir_led_task, "pir_led_task", 4096, &pir, 5, NULL);
    xTaskCreate(dht_task, "dht_task", 4096, &pir, 5, NULL);
    xTaskCreate(thermistor_task, "thermistor_task", 4096, &pir, 5, NULL);

    nvs_logger_init();
    
    while (1)
    {
        char telemetry_data[128];

        check_button_and_sleep();
        message_receiver(mosquitto);

        if (pir.temperature > maxTemperature) {
            maxTemperature = pir.temperature;
        }
        else if (pir.temperature < minTemperature) {
            minTemperature = pir.temperature;
        }

        if (pir.humidity > maxHumidity) {
            maxHumidity = pir.humidity;
        }
        else if (pir.humidity < minHumidity) {
            minHumidity = pir.humidity;
        };

        sprintf(telemetry_data, "{\"temperatura\":%.1f,\"maxTemperatura\":%.1f,\"minTemperatura\":%.1f,\"umidade\":%.1f,\"maxUmidade\":%.1f,\"minUmidade\":%.1f}", pir.temperature, maxTemperature, minTemperature, pir.humidity, maxHumidity, minHumidity);
        ESP_LOGI(TAG, "TEMPERATURA E UMIDADE: %f %f", pir.temperature, pir.humidity);
        esp_mqtt_client_publish(client, "v1/devices/me/telemetry", telemetry_data, 0, 1, 0);

        // Wait for 10 seconds before next read
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}