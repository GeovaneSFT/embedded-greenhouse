#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "wifi.h"

#define TAG_MQTT "MQTT"

esp_mqtt_client_handle_t client_mosquitto;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG_MQTT, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG_MQTT, "Event dispatched from event loop base=%s, event_id=%d", base, (int) event_id);

    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
            ESP_LOGI(TAG_MQTT, "Sent subscribe with success, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGW(TAG_MQTT, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGW(TAG_MQTT, "MQTT_EVENT_ERROR: %d!", event->error_handle->error_type);
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG_MQTT, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

            }
            break;
        default:
            ESP_LOGI(TAG_MQTT, "Other event id:%d", event->event_id);
            break;
    }
}

void mqtt_start() {

    // mqtt mosquitto
    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri =  "mqtt://test.mosquitto.org"
    };
    client_mosquitto = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client_mosquitto, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client_mosquitto);
    ESP_LOGI(TAG_MQTT, "Conectei no mosquitto");
}

void mqtt_envia_mensagem_mosquitto(char * topico, char * mensagem) {
    int message_id = esp_mqtt_client_publish(client_mosquitto, topico, mensagem, 0, 1, 0);
    ESP_LOGI(TAG_MQTT, "Mensagem enviada, ID: %d", message_id);
}


void app_main(void * params) {


    wifi_start();
    sleep(5);
    mqtt_start();
    char mensagem[100];


    float minTemperature = 0.0;
    float minHumidity = 0.0;

        while (1) {

            minTemperature = 15;
            minHumidity = 23;

            sprintf(mensagem, "{\"minTemperatura\":%.1f,\"minUmidade\":%.1f}",minTemperature, minHumidity);

            mqtt_envia_mensagem_mosquitto("verificador_umidade/sensores/placa1", mensagem);
            sleep(2);

        }
    
}