#ifndef SMQTT
#define SMQTT


#ifdef __cplusplus
extern "C" {
#endif

#include <Arduino.h>

#include <mqtt_config.h>
#include <mqtt_client.h>

#define HOST "192.168.42.1"
#define PORT 1883
#define USERNAME "mosquitto"
#define PASSWORD "dietpi"

#define ID "test"

#define FRQ 100

esp_mqtt_client_handle_t setup_mqtt();
void monitor_mqtt(void *param);

void send_value();
void callback();

#ifdef __cplusplus
}
#endif

#endif