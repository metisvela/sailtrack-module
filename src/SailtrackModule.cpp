#include "SailtrackModule.h"

PubSubClient * SailtrackModule::mqtt = NULL;

void SailtrackModule::init(const char * name) {
    Serial.begin(115200);
    Serial.println();
    initWifi(name);
    initMqtt(name);
}

void SailtrackModule::initWifi(const char * hostname) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);
    WiFi.begin(WIFI_SSID, WIFI_PSW);

    esp_sleep_enable_timer_wakeup(WIFI_SLEEP_DURATION);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("Impossible to connect to '%s'\n", WIFI_SSID);
        Serial.println("Going to deep slep, goodnight...");
        esp_deep_sleep_start();
    }
    Serial.printf("Successfully connected to '%s'!\n", WIFI_SSID);
    Serial.printf("IP Address: %s\n", WiFi.localIP());
    
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.printf("Lost connection to '%s'\n", WIFI_SSID);
        Serial.println("Going to deep sleep, goodnight...");
        esp_deep_sleep_start();
    }, SYSTEM_EVENT_STA_DISCONNECTED);
}

void SailtrackModule::initMqtt(const char * name) {
    WiFiClient wifiClient;
    mqtt = new PubSubClient(MQTT_SERVER_IP, MQTT_PORT, wifiClient);
    mqtt->connect(name, MQTT_USERNAME, MQTT_PASSWORD);
    Serial.println("MQTT Connected!");
}

void SailtrackModule::loop() {
    mqtt->loop();
}
