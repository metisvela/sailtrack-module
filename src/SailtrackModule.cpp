#include "SailtrackModule.h"

esp_mqtt_client_handle_t SailtrackModule::mqttClient;
esp_mqtt_client_config_t SailtrackModule::mqttConfig;
SailtrackModule STModule;

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
        Serial.println("Going to deep sleep, goodnight...");
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
    mqttConfig.host = MQTT_SERVER_IP;
    mqttConfig.port = MQTT_PORT;
    mqttConfig.username = MQTT_USERNAME;
    mqttConfig.password = MQTT_PASSWORD;
    mqttClient = esp_mqtt_client_init(&mqttConfig);

    Serial.print("Connecting to MQTT server...");
    for (int i = 0; esp_mqtt_client_start(mqttClient) && i < 5; i++) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();

    Serial.println("Successfully connected to MQTT server!");
}

int SailtrackModule::publish(const char * topic, const char * measurement, DynamicJsonDocument & payload) {
    DynamicJsonDocument message(measureJson(payload) + JSON_OBJECT_SIZE(1));
    message = payload;
    message["measurement"] = measurement;
    int size = measureJson(message);
    char output[size];
    serializeJson(message, output, size);
    return esp_mqtt_client_publish(mqttClient, topic, output, 0, 1, 0);
}

int SailtrackModule::subscribe(const char * topic) {
    return esp_mqtt_client_subscribe(mqttClient, topic, 1);
}

esp_err_t SailtrackModule::registerEvent(esp_mqtt_event_id_t event, esp_event_handler_t eventHandler, void * eventHandlerArg) {
    return esp_mqtt_client_register_event(mqttClient, event, eventHandler, eventHandlerArg);
}
