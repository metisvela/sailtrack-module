#include "SailtrackModule.h"

void SailtrackModule::init(const char * name) {
    Serial.begin(115200);

    initWifi(name);
    initMqtt(name);
}

void SailtrackModule::initWifi(const char * hostname) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);
    WiFi.begin(WIFI_SSID, WIFI_PSW);

    esp_sleep_enable_timer_wakeup(WIFI_SLEEP_DURATION);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Impossible to connect to WiFi network, going to deep sleep, goodnight!");
        esp_deep_sleep_start();
    }
    Serial.println("WiFi Connected!");
    
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println("Lost connection to WiFi, going to deep sleep, goodnight!");
        esp_deep_sleep_start();
    }, SYSTEM_EVENT_STA_DISCONNECTED);
}

void SailtrackModule::initMqtt(const char * name) {
    WiFiClient wifiClient;
    mqtt = new PubSubClient(MQTT_SERVER_IP, MQTT_PORT, wifiClient);
    mqtt->connect(name, MQTT_USERNAME, MQTT_PASSWORD);
    mqtt->publish("test", "hi bro");
    Serial.println("MQTT Connected!");
}
