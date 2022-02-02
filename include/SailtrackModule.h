#ifndef SAILTRACK_MODULE_H
#define SAILTRACK_MODULE_H

#define USE_ESP_IDF_LOG
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include <Arduino.h>
#include <WiFi.h>
#include <mqtt_client.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include "SailtrackModuleConfig.h"
#include "SailtrackModuleCallbacks.h"

class SailtrackModule {
    static const char * name;
    static const char * hostname;
    static IPAddress ipAddress;
    static SailtrackModuleCallbacks * callbacks;
    static esp_mqtt_client_config_t mqttConfig;
    static esp_mqtt_client_handle_t mqttClient;
    static bool mqttConnected;
    static int publishedMessagesCount;
    static void beginLogging();
    static void beginWifi();
    static void beginOTA();
    static void beginMqtt();
    static void mqttEventHandler(void * handlerArgs, esp_event_base_t base, int32_t eventId, void * eventData);
    static void statusTask(void * pvArguments);
    static void logTask(void * pvArguments);
    static void otaTask(void * pvArguments);
    static int m_vprintf(const char * format, va_list args);
    public:
        static void begin(const char * name, const char * hostname, IPAddress ipAddress);
        static void setCallbacks(SailtrackModuleCallbacks * callbacks);
        static int publish(const char * topic, const char * measurement, DynamicJsonDocument payload);
        static int subscribe(const char * topic);
};

extern SailtrackModule STModule;

#endif
