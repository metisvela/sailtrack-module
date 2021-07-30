#include "SailtrackModule.h"

const char * SailtrackModule::name;
const char * SailtrackModule::hostname;
IPAddress SailtrackModule::ipAddress;

SailtrackModuleCallbacks * SailtrackModule::callbacks;

esp_mqtt_client_config_t SailtrackModule::mqttConfig;
esp_mqtt_client_handle_t SailtrackModule::mqttClient;
bool SailtrackModule::mqttConnected;

void SailtrackModule::begin(const char * name, const char * hostname, IPAddress ipAddress) {
    SailtrackModule::name = name;
    SailtrackModule::hostname = hostname;
    SailtrackModule::ipAddress = ipAddress;
    beginLogging();
    beginWifi();
    beginOTA();
    beginMqtt();
}

void SailtrackModule::beginLogging() {
    Serial.begin(115200);
    Serial.println();
    esp_log_set_vprintf(m_vprintf);
    esp_log_level_set(TAG, ESP_LOG_INFO);
}

void SailtrackModule::beginWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);
    WiFi.config(ipAddress, WIFI_GATEWAY, WIFI_SUBNET);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    if (callbacks) callbacks->onWifiConnectionBegin();
    
    ESP_LOGI(TAG, "Connecting to '%s'...", WIFI_SSID);

    for (int i = 0; WiFi.status() != WL_CONNECTED && i < WIFI_CONNECTION_TIMEOUT_MS / 500 ; i++)
        delay(500);

    if (WiFi.status() != WL_CONNECTED) {
        ESP_LOGE(TAG, "Impossible to connect to '%s'", WIFI_SSID);
        ESP_LOGE(TAG, "Going to deep sleep, goodnight...");
        ESP.deepSleep(WIFI_SLEEP_DURATION_US);
    }

    if (callbacks) callbacks->onWifiConnectionResult(WiFi.status());

    ESP_LOGI(TAG, "Successfully connected to '%s'!", WIFI_SSID);

    WiFi.onEvent([](WiFiEvent_t event) {
        ESP_LOGE(TAG, "Lost connection to '%s'", WIFI_SSID);
        if (callbacks) callbacks->onWifiDisconnected();
        ESP_LOGE(TAG, "Rebooting...");
        ESP.restart();
    }, SYSTEM_EVENT_STA_DISCONNECTED);
}

void SailtrackModule::beginOTA() {
    ArduinoOTA
        .onStart([]() {
            if (ArduinoOTA.getCommand() == U_FLASH) ESP_LOGI(TAG, "Start updating sketch...");
            else ESP_LOGI(TAG, "Start updating filesystem...");
        })
        .onEnd([]() {
            ESP_LOGI(TAG, "Update successfully completed!");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            ESP_LOGV(TAG, "Progress: %u", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            if (error == OTA_AUTH_ERROR) ESP_LOGE(TAG, "Error[%u]: Auth Failed", error);
            else if (error == OTA_BEGIN_ERROR) ESP_LOGE(TAG, "Error[%u]: Begin Failed", error);
            else if (error == OTA_CONNECT_ERROR) ESP_LOGE(TAG, "Error[%u]: Connect Failed", error);
            else if (error == OTA_RECEIVE_ERROR) ESP_LOGE(TAG, "Error[%u]: Receive Failed", error);
            else if (error == OTA_END_ERROR) ESP_LOGE(TAG, "Error[%u]: End Failed", error);
        });
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.begin();
}

void SailtrackModule::beginMqtt() {
    mqttConfig.host = MQTT_SERVER;
    mqttConfig.port = MQTT_PORT;
    mqttConfig.username = MQTT_USERNAME;
    mqttConfig.password = MQTT_PASSWORD;
    mqttConfig.client_id = hostname;
    mqttClient = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_start(mqttClient);
    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_CONNECTED, mqttEventHandler, NULL);

    ESP_LOGI(TAG, "Connecting to 'mqtt://%s@%s:%d'...", MQTT_USERNAME, MQTT_SERVER, MQTT_PORT);

    for (int i = 0; !mqttConnected && i < MQTT_CONNECTION_TIMEOUT_MS / 500; i++)
        delay(500);

    if (!mqttConnected) {
        ESP_LOGE(TAG, "Impossible to connect to 'mqtt://%s@%s:%d'", MQTT_USERNAME, MQTT_SERVER, MQTT_PORT);
        ESP_LOGE(TAG, "Rebooting...");
        ESP.restart();
    }

    ESP_LOGI(TAG, "Successfully connected to 'mqtt://%s@%s:%d'!", MQTT_USERNAME, MQTT_SERVER, MQTT_PORT);

    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_DATA, mqttEventHandler, NULL);
    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_DISCONNECTED, mqttEventHandler, NULL);
}

void SailtrackModule::mqttEventHandler(void * handlerArgs, esp_event_base_t base, int32_t eventId, void * eventData) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)eventData;
    switch((esp_mqtt_event_id_t)eventId) {
        case MQTT_EVENT_CONNECTED:
            if (callbacks) callbacks->onMqttConnected();
            mqttConnected = true;
            break;
        case MQTT_EVENT_DATA:
            if (callbacks) callbacks->onMqttMessage(event->topic, event->data);
            break;
        case MQTT_EVENT_DISCONNECTED:
            if (callbacks) callbacks->onMqttDisconnected();
            mqttConnected = false;
            break;
        default:
            break;
    }
}

void SailtrackModule::setCallbacks(SailtrackModuleCallbacks * callbacks) {
    SailtrackModule::callbacks = callbacks;
    xTaskCreate(statusTask, "status_task", TASK_MEDIUM_STACK_SIZE, NULL, TASK_MEDIUM_PRIORITY, NULL);
}

void SailtrackModule::statusTask(void * pvArguments) {
    char topic[50];
    sprintf(topic, "module/%s", name);

    while(true) {
        DynamicJsonDocument payload = callbacks->getStatus();
        publish(topic, name, payload);
        delay(STATUS_PUBLISH_PERIOD_MS);
    }
}

int SailtrackModule::m_vprintf(const char * format, va_list args) {
    if (mqttConnected) {
        char message[200];
        char topic[50];
        int messageSize;
        sprintf(topic, "module/%s", name);
        vsprintf(message, format, args);
        for (messageSize = 0; message[messageSize]; messageSize++);
        message[messageSize - 1] = 0;
        DynamicJsonDocument payload(500);
        JsonObject log = payload.createNestedObject("log");
        log["message"] = message;
        publish(topic, name, payload);
    }
    return vprintf(format, args);
}

int SailtrackModule::publish(const char * topic, const char * measurement, DynamicJsonDocument payload) {
    payload["measurement"] = measurement;
    char output[MQTT_OUTPUT_BUFFER_SIZE];
    serializeJson(payload, output);
    return esp_mqtt_client_publish(mqttClient, topic, output, 0, 1, 0);
}

int SailtrackModule::subscribe(const char * topic) {
    return esp_mqtt_client_subscribe(mqttClient, topic, 1);
}

void SailtrackModule::loop() {
    ArduinoOTA.handle();
}

SailtrackModule STModule;
