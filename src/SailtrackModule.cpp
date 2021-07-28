#include "SailtrackModule.h"

const char * SailtrackModule::name;
const char * SailtrackModule::hostname;
IPAddress SailtrackModule::ipAddress;
SailtrackModuleCallbacks * SailtrackModule::callbacks;
esp_mqtt_client_config_t SailtrackModule::mqttConfig;
esp_mqtt_client_handle_t SailtrackModule::mqttClient;
TaskHandle_t SailtrackModule::waitingTask;

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
    esp_log_level_set(TAG, ESP_LOG_INFO);
}

void SailtrackModule::beginWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);
    WiFi.config(ipAddress, WIFI_GATEWAY, WIFI_SUBNET);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    if (callbacks) callbacks->onWifiConnectionBegin();
    
    ESP_LOGI(TAG, "Connecting to '%s'...", WIFI_SSID);

    WiFi.onEvent(wifiEventHandler, SYSTEM_EVENT_STA_CONNECTED);

    waitingTask = xTaskGetCurrentTaskHandle();
    if (!xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(WIFI_CONNECTION_TIMEOUT_MS))) {
        ESP_LOGE(TAG, "Impossible to connect to '%s'", WIFI_SSID);
        ESP_LOGE(TAG, "Going to deep sleep, goodnight...");
        ESP.deepSleep(WIFI_SLEEP_DURATION_US);
    }

    if (callbacks) callbacks->onWifiConnectionResult(WiFi.status());

    WiFi.onEvent(wifiEventHandler, SYSTEM_EVENT_STA_DISCONNECTED);
}

void SailtrackModule::beginOTA() {
    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH) type = "sketch";
            else type = "filesystem";
            Serial.println("Start updating " + type);
        })
        .onEnd([]() {
            Serial.println("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
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

    ESP_LOGI(TAG, "Connecting to 'mqtt://%s@%s:%d'...", MQTT_USERNAME, MQTT_SERVER, MQTT_PORT);

    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_CONNECTED, mqttEventHandler, NULL);

    esp_mqtt_client_start(mqttClient);
    waitingTask = xTaskGetCurrentTaskHandle();
    if (!xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(MQTT_CONNECTION_TIMEOUT_MS))) {
        ESP_LOGE(TAG, "Impossible to connect to 'mqtt://%s@%s:%d'", MQTT_USERNAME, MQTT_SERVER, MQTT_PORT);
        ESP_LOGE(TAG, "Restarting...");
        ESP.restart();
    }

    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_DISCONNECTED, mqttEventHandler, NULL);
    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_DATA, mqttEventHandler, NULL);
}

void SailtrackModule::wifiEventHandler(WiFiEvent_t event) {
    switch(event) {
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "Successfully connected to '%s'!", WIFI_SSID);
            xTaskNotify(waitingTask, 0, eNoAction);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGE(TAG, "Lost connection to '%s'", WIFI_SSID);
            ESP_LOGE(TAG, "Attempting reconnection...");
            WiFi.reconnect();
            waitingTask = xTaskGetCurrentTaskHandle();
            if (!xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(WIFI_RECONNECTION_TIMEOUT_MS))) {
                ESP_LOGE(TAG, "Impossible to reconnect to '%s'", WIFI_SSID);
                ESP_LOGE(TAG, "Restarting...");
                ESP.restart();
            }
            break;
        default:
            break;
    }
}

void SailtrackModule::mqttEventHandler(void * handlerArgs, esp_event_base_t base, int32_t eventId, void * eventData) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)eventData;
    switch((esp_mqtt_event_id_t)eventId) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Successfully connected to 'mqtt://%s@%s:%d'!", MQTT_USERNAME, MQTT_SERVER, MQTT_PORT);
            xTaskNotify(waitingTask, 0, eNoAction);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGE(TAG, "Lost connection to 'mqtt://%s@%s:%d'", MQTT_USERNAME, MQTT_SERVER, MQTT_PORT);
            ESP_LOGE(TAG, "Attempting reconnection...");
            esp_mqtt_client_reconnect(mqttClient);
            waitingTask = xTaskGetCurrentTaskHandle();
            if (!xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(MQTT_RECONNECTION_TIMEOUT_MS))) {
                ESP_LOGE(TAG, "Impossible to reconnect to 'mqtt://%s@%s:%d'", MQTT_USERNAME, MQTT_SERVER, MQTT_PORT);
                ESP_LOGE(TAG, "Restarting...");
                ESP.restart();
            }
            break;
        case MQTT_EVENT_DATA:
            if (callbacks) callbacks->onMqttMessage(event->topic, event->data);
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

int SailtrackModule::publish(const char * topic, const char * measurement, DynamicJsonDocument payload) {
    payload["measurement"] = measurement;
    char output[500];
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
