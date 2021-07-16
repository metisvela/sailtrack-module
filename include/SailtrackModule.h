#ifndef SAILTRACK_MODULE_H
#define SAILTRACK_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <mqtt_config.h>
#include <mqtt_client.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include "SailtrackConfig.h"

class SailtrackModule {
    private:
        static esp_mqtt_client_config_t mqttConfig;
        static esp_mqtt_client_handle_t mqtt;
        static void initWifi(const char * hostname);
        static void initMqtt(const char * name);
    public:
        static void init(const char * name);
        static int publish(const char * topic, const char * payload);
};

#endif
