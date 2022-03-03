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
    static int notificationLed;
    static WifiConfig wifiConfig;
    static esp_mqtt_client_config_t mqttConfig;
    static esp_mqtt_client_handle_t mqttClient;
    static bool mqttConnected;
    static int publishedMessagesCount;
    static int receivedMessagesCount;

    static void beginNotificationLed();
    static void beginLogging();
    static void beginWifi();
    static void beginOTA();
    static void beginMqtt();
    static void mqttEventHandler(void * handlerArgs, esp_event_base_t base, int32_t eventId, void * eventData);
    static void notificationLedTask(void * pvArguments);
    static void statusTask(void * pvArguments);
    static void logTask(void * pvArguments);
    static void otaTask(void * pvArguments);
    static int m_vprintf(const char * format, va_list args);

    public:
        static void configWifi(const char * ssid, const char * password, IPAddress gateway, IPAddress subnet);
        static void configMqtt(IPAddress host, int port, const char * username, const char * password);
        #ifdef LED_BUILTIN
        static void begin(const char * name, IPAddress ip, SailtrackModuleCallbacks * callbacks = NULL, int notificationLed = LED_BUILTIN);
        #else
        static void begin(const char * name, IPAddress ip, SailtrackModuleCallbacks * callbacks = NULL, int notificationLed = -1);
        #endif
        static int publish(const char * topic, DynamicJsonDocument payload);
        static int subscribe(const char * topic);
};

#endif
