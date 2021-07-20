#ifndef SAILTRACK_MODULE_H
#define SAILTRACK_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <mqtt_client.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include "SailtrackConfig.h"

class SailtrackModule {
    private:
        static esp_mqtt_client_config_t mqttConfig;
        static esp_mqtt_client_handle_t mqttClient;
        static void initWifi(const char * hostname, IPAddress ip);
        static void initMqtt(const char * name);
        static void initOTA();
    public:
        static void init(const char * name, IPAddress ip);
        static void loop();
        static int publish(const char * topic, const char * mesaurement, DynamicJsonDocument & payload);
        static int subscribe(const char * topic);
        static esp_err_t registerEvent(esp_mqtt_event_id_t event, esp_event_handler_t eventHandler, void * eventHandlerArg);
};

extern SailtrackModule STModule;

#endif
