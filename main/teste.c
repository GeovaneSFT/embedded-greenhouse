// #include <stdio.h>
// #include <stdint.h>
// #include <stddef.h>
// #include <string.h>
// #include "esp_system.h"
// #include "esp_event.h"
// #include "esp_netif.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/semphr.h"
// #include "freertos/queue.h"
// #include "lwip/sockets.h"
// #include "lwip/dns.h"
// #include "lwip/netdb.h"
// #include "esp_log.h"
// #include "mqtt_client.h"
// #include "mqtt.h"
// #include "cJSON.h"
// #define LED_1 3
// #define TAG "MQTT"
// extern int estadoporta;
// extern int estado ;
// extern int estadoluz;
// //int estadoluz = 0;
// extern SemaphoreHandle_t conexaoMQTTSemaphore;
// esp_mqtt_client_handle_t client;
// esp_mqtt_client_handle_t client2;
// static void log_error_if_nonzero(const char *message, int error_code)
// {
// if (error_code != 0) {
// ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
// }
// }
// static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t
// event_id, void *event_data)
// {
// ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, (int)
// event_id);
// esp_mqtt_event_handle_t event = event_data;
// esp_mqtt_client_handle_t client = event->client;
// int msg_id;
// switch ((esp_mqtt_event_id_t)event_id) {
// case MQTT_EVENT_CONNECTED:
// ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
// xSemaphoreGive(conexaoMQTTSemaphore);
// msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
// break;
// case MQTT_EVENT_DISCONNECTED:
// ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
// break;
// case MQTT_EVENT_SUBSCRIBED:
// ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
// msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
// ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
// break;
// case MQTT_EVENT_UNSUBSCRIBED:
// ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
// break;
// case MQTT_EVENT_PUBLISHED:
// ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
// break;
// case MQTT_EVENT_DATA:
// ESP_LOGI(TAG, "MQTT_EVENT_DATA");
// printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
// printf("DATA=%.*s\r\n", event->data_len, event->data);
// cJSON *json = cJSON_Parse(event->data);
// if (json == NULL) {
// fprintf(stderr, "Error: %s\n", cJSON_GetErrorPtr());
// } else {
// // Extrai o valor da chave "method"
// cJSON *method_item = cJSON_GetObjectItem(json, "method");
// if (method_item != NULL && cJSON_IsString(method_item)) {
// char *chave = method_item->valuestring;
// // Extrai o valor da chave "params"
// cJSON *params_item = cJSON_GetObjectItem(json, "params");
// if (params_item != NULL && (cJSON_IsString(params_item)||
// cJSON_IsNumber(params_item)) ){
// char *params_str = params_item->valuestring;
// printf("Chave: %s\n", chave);
// if (strcmp("Value", chave ) == 0) {
// // Extrai o valor do item "params" do JSON
// cJSON *params_item = cJSON_GetObjectItem(json, "params");
// if (params_item != NULL && cJSON_IsNumber(params_item)) {
// // Converte o valor para inteiro e armazena em estadoluz
// estadoluz = (int)params_item->valuedouble;
// printf("Valor de estadoLuz: %d\n", estadoluz);
// } else {
// printf("Erro: params não é um número válido.\n");
// }
// }
// // Verifica se a chave "method" é "setLuz"
// else if (strcmp("setLuz", chave) == 0) {
// if (strstr(params_str, "\"status da tranca\" = true") != NULL && estadoporta == 0) {
// estado =1 ;
// ESP_LOGI(TAG, "LED Ligado");
// } else if (strstr(params_str, "\"status da tranca\" = false") != NULL && estadoporta
// == 0) {
// estado =0 ;
// ESP_LOGI(TAG, "LED Desligado");
// }
// }
// }
// }
// cJSON_Delete(json);
// }
// break;
// case MQTT_EVENT_ERROR:
// ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
// if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
// log_error_if_nonzero("reported from esp-tls",
// event->error_handle->esp_tls_last_esp_err);
// log_error_if_nonzero("reported from tls stack",
// event->error_handle->esp_tls_stack_err);
// log_error_if_nonzero("captured as transport's socket errno",
// event->error_handle->esp_transport_sock_errno);
// ESP_LOGI(TAG, "Last errno string (%s)",
// strerror(event->error_handle->esp_transport_sock_errno));
// }
// break;
// default:
// ESP_LOGI(TAG, "Other event id:%d", event->event_id);
// break;
// }
// }
// static void mosquitto_event_handler(void *handler_args, esp_event_base_t base, int32_t
// event_id, void *event_data)
// {
// ESP_LOGD(TAG, "Mosquitto Event dispatched from event loop base=%s, event_id=%d",
// base, (int) event_id);
// esp_mqtt_event_handle_t event = event_data;
// esp_mqtt_client_handle_t client = event->client;
// switch ((esp_mqtt_event_id_t)event_id) {
// case MQTT_EVENT_CONNECTED:
// ESP_LOGI(TAG, "Mosquitto MQTT_EVENT_CONNECTED");
// esp_mqtt_client_subscribe(client, "fse-2024/cabs/mag", 0);
// break;
// case MQTT_EVENT_DATA:
// ESP_LOGI(TAG, "Mosquitto MQTT_EVENT_DATA");
// printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
// printf("DATA=%.*s\r\n", event->data_len, event->data);
// cJSON *json = cJSON_Parse(event->data);
// if (json == NULL)
// {
// fprintf(stderr, "Error: %s\n", cJSON_GetErrorPtr());
// }
// else
// {
// // Extrai o valor da chave "status do mag"
// cJSON *status_item = cJSON_GetObjectItem(json, "status do mag");
// if (status_item != NULL && cJSON_IsBool(status_item))
// {
// bool status = cJSON_IsTrue(status_item);
// if (!status && estadoporta == 0)
// {
// // estado = 1;
// // ESP_LOGI(TAG, "LED Ligado");
// }
// else if (status && estadoporta == 0)
// {
// estado = 0;
// ESP_LOGI(TAG, "LED Desligado");
// }
// }
// else
// {
// ESP_LOGW(TAG, "Chave 'status do mag' não encontrada ou não é um
// booleano.");
// }
// cJSON_Delete(json);
// }
// break;
// default:
// ESP_LOGI(TAG, "Mosquitto Other event id:%d", event->event_id);
// break;
// }
// }
// void mqtt_start()
// {
// esp_mqtt_client_config_t mqtt_config = {
// .broker.address.uri = "mqtt://164.41.98.25",
// .credentials.username = "WKjY0k489lO8YMkbjyj6",
// };
// client = esp_mqtt_client_init(&mqtt_config);
// esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler,
// NULL);
// esp_mqtt_client_start(client);
// esp_mqtt_client_config_t mosquitto_config = {
// .broker.address.uri = "mqtt://test.mosquitto.org",
// // .credentials.username = "WKjY0k489lO8YMkbjyj6",
// };
// client2 = esp_mqtt_client_init(&mosquitto_config);
// esp_mqtt_client_register_event(client2, ESP_EVENT_ANY_ID, mosquitto_event_handler,
// NULL);
// esp_mqtt_client_start(client2);
// }
// void mosquitto_envia_mensagem(char * topico, char * mensagem)
// {
// int message_id2 = esp_mqtt_client_publish(client2, topico, mensagem, 0, 1, 0);
// ESP_LOGI(TAG, "Mesnagem enviada, ID: %d", message_id2);
// }
// void mqtt_envia_mensagem(char * topico, char * mensagem)
// {
// int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
// ESP_LOGI(TAG, "Mesnagem enviada, ID: %d", message_id);
// }