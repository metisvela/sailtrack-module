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

struct WifiConfig {
    const char * hostname;
    const char * ssid;
    const char * password;
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;
};

class SailtrackModule {
    static const char * name;
    static SailtrackModuleCallbacks * callbacks;
    static WifiConfig wifiConfig;
    static esp_mqtt_client_config_t mqttConfig;
    static esp_mqtt_client_handle_t mqttClient;
    static bool mqttConnected;
    static int publishedMessagesCount;
    static int receivedMessagesCount;
    static void beginLogging();
    static void beginWifi(IPAddress ip);
    static void beginOTA();
    static void beginMqtt();
    static void mqttEventHandler(void * handlerArgs, esp_event_base_t base, int32_t eventId, void * eventData);
    static void statusTask(void * pvArguments);
    static void logTask(void * pvArguments);
    static void otaTask(void * pvArguments);
    static int m_vprintf(const char * format, va_list args);
    public:
        static void configWifi(const char * ssid, const char * password, IPAddress gateway, IPAddress subnet);
        static void configMqtt(IPAddress host, int port, const char * username, const char * password);
        static void begin(const char * name, IPAddress ip);
        static void setCallbacks(SailtrackModuleCallbacks * callbacks);
        static int publish(const char * topic, DynamicJsonDocument payload);
        static int subscribe(const char * topic);
};

#endif
