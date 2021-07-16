#include "SailtrackModule.h"

WiFiClient SailtrackModule::wifiClient;
PubSubClient SailtrackModule::mqtt;

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

    Serial.printf("Connecting to '%s'...", WIFI_SSID);
    for (int i = 0; WiFi.waitForConnectResult() != WL_CONNECTED && i < 5; i++) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.printf("Impossible to connect to '%s'\n", WIFI_SSID);
        Serial.println("Going to deep slep, goodnight...");
        esp_deep_sleep_start();
    }

    Serial.printf("Successfully connected to '%s'!\n", WIFI_SSID);
    
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.printf("Lost connection to '%s'\n", WIFI_SSID);
        Serial.println("Going to deep sleep, goodnight...");
        esp_deep_sleep_start();
    }, SYSTEM_EVENT_STA_DISCONNECTED);
}

void SailtrackModule::initMqtt(const char * name) {
    mqtt = PubSubClient(MQTT_SERVER_IP, MQTT_PORT, wifiClient);

    Serial.print("Connecting to MQTT server...");
    for (int i = 0; !mqtt.connect(name, MQTT_USERNAME, MQTT_PASSWORD) && i < 5; i++) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();

    Serial.println("Successfully connected to MQTT server!");
}

void SailtrackModule::loop() {
    mqtt.loop();
}

void SailtrackModule::publish(const char * topic, const char * payload) {
    mqtt.publish(topic, payload);
}
