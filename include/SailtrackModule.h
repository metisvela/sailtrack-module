#ifndef SAILTRACK_MODULE_H
#define SAILTRACK_MODULE_H

#include "SailtrackModuleConfig.h"
#include <Arduino.h>
#include <WiFi.h>
#include <mqtt_client.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "SailtrackModuleCallbacks.h"

class SailtrackModule {
    static const char * name;
    static const char * hostname;
    static IPAddress ipAddress;
    static SailtrackModuleCallbacks * callbacks;
    static esp_mqtt_client_config_t mqttConfig;
    static esp_mqtt_client_handle_t mqttClient;
    static TaskHandle_t waitingTask;
    static void beginLogging();
    static void beginWifi();
    static void beginOTA();
    static void beginMqtt();
    static void wifiEventHandler(WiFiEvent_t event);
    static void mqttEventHandler(void * handlerArgs, esp_event_base_t base, int32_t eventId, void * eventData);
    static void statusTask(void * pvArguments);
    public:
        static void begin(const char * name, const char * hostname, IPAddress ipAddress);
        static void setCallbacks(SailtrackModuleCallbacks * callbacks);
        static void loop();
        static int publish(const char * topic, const char * measurement, DynamicJsonDocument payload);
        static int subscribe(const char * topic);
};

extern SailtrackModule STModule;

#endif
