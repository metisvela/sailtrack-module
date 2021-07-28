#ifndef SAILTRACK_MODULE_H
#define SAILTRACK_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <mqtt_client.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#ifdef ARDUINO_T_Beam
#include <axp20x.h>
#endif

#include "SailtrackConfig.h"

class SailtrackModule {
    private:
        static esp_mqtt_client_config_t mqttConfig;
        static esp_mqtt_client_handle_t mqttClient;
        static const char * name;
        static char hostname[];
        static void initWifi(IPAddress ip);
        static void initMqtt();
        static void initOTA();
        static void initPower();
        static void statusTask(void * pvArguments);
    public:
        static void init(const char * name, IPAddress ip);
        static void loop();
        static int publish(const char * topic, const char * mesaurement, JsonDocument & payload);
        static int subscribe(const char * topic);
        static esp_err_t registerEvent(esp_mqtt_event_id_t event, esp_event_handler_t eventHandler, void * eventHandlerArg);
};

extern SailtrackModule STModule;

#endif
