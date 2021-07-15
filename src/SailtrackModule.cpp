#include "SailtrackModule.h"

void SailtrackModule::initWifi(const char * hostname) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);
    WiFi.begin(WIFI_SSID, WIFI_PSW);

    esp_sleep_enable_timer_wakeup(WIFI_SLEEP_DURATION);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Impossible to connect to WiFi network, going to deep sleep, goodnight!");
        esp_deep_sleep_start();
    }
    Serial.println("WiFi connected!");
    
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println("Lost connection to WiFi, going to deep sleep, goodnight!");
        esp_deep_sleep_start();
    }, SYSTEM_EVENT_STA_DISCONNECTED);
}

SailtrackModule::SailtrackModule(const char * name) {
    initWifi(name);
}
