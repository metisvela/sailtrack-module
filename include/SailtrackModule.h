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

struct NotificationLed {
    int pin;
    bool reversed;
};

struct WifiConfig {
    const char * hostname;
    const char * ssid;
    const char * password;
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;
};

/**
 * Main static class and entrypoint of the `SailtrackModule` library.
 */
class SailtrackModule {
    static const char * name;
    static SailtrackModuleCallbacks * callbacks;
    static NotificationLed * notificationLed;
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

        /**
         * Set the WiFi configuration. This method is particularly useful to connect to a custom WiFi network, different 
         * from the default one (e.g. for testing/development purposes). The default configuration can be found in 
         * `SailtrackModuleConfig.h` under the 'WiFi Configuration' section.
         * 
         * @param ssid The SSID of the WiFi network.
         * @param password The password of the WiFi network.
         * @param gateway The IP address of the gateway of the WiFi network.
         * @param subnet The subnet of the WiFi network. Ususally equal to `WIFI_DEFAULT_SUBNET`.
         */
        static void configWifi(const char * ssid, const char * password, IPAddress gateway, IPAddress subnet);

        /**
         * Set the MQTT configuration. This method is particularly useful to connect to a custom MQTT broker, different
         * from the default one (e.g. for testing/development purposes). The default configuration can be found in 
         * `SailtrackModuleConfig.h` under the 'MQTT Configuration' section.
         * 
         * @param host The IP address of the MQTT broker.
         * @param port The port of the MQTT broker. Usually equal to `MQTT_DEFAULT_PORT`.
         * @param username The username used to authenticate to the MQTT broker. Leave empty if authentication is not 
         *      required.
         * @param password The password used to authenticate to the MQTT broker. Leave empty if authentication is not
         *      required.
         */ 
        static void configMqtt(IPAddress host, int port, const char * username = "", const char * password = "");

        /**
         * Set the notification LED. It will be used to notify the user (e.g. for the connection status).
         * 
         * @param pin The pin number the notification LED is connected to.
         * @param reversed Set to `true` if the LED is reversed (`pin` connected to the cathode of the LED). If 
         *      reversed, the LED will be turned on by setting `pin` to `LOW`.
         */
        static void setNotificationLed(int pin, bool reversed = false);

        /**
         * Initialize the module and the peripherals, connecting it to the WiFi network and the MQTT broker. This method
         * must be called in the `setup()` function in order to correctly use the library.
         * 
         * @param name The name of the module. Used in log messages, netowork names, database entries,... .
         * @param ip The static IP address to assign to the module. Make sure that this IP does not collide with others
         *      in the network.
         * @param callbacks A pointer to the `SailtrackModuleCallbacks` callbacks class. Methods in this class will be
         *      called from inside the library to notify and request values from the main application.
         */
        static void begin(const char * name, IPAddress ip, SailtrackModuleCallbacks * callbacks);

        /**
         * Publish data to MQTT. Use this method to publish a JSON formatted payload to the given MQTT topic.
         * 
         * @param topic The topic `payload` will be published to.
         * @param payload The payload to publish formatted as a JSON document.
         * @return The ID of the published message on success, -1 on failure.
         */
        static int publish(const char * topic, DynamicJsonDocument * payload);

        /**
         * Subscribe to a MQTT topic. To notify new messages, the `onMqttMessage(...)` callback will be used.
         * 
         * @param topic The topic name to subscribe to.
         * @return The ID of the subscribe message on success, -1 on failure.
         */ 
        static int subscribe(const char * topic);
};

#endif
