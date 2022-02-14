#include "SailtrackModule.h"

static const char * LOG_TAG = "SAILTRACK_MODULE";

const char * SailtrackModule::name;

SailtrackModuleCallbacks * SailtrackModule::callbacks;

WifiConfig SailtrackModule::wifiConfig;

esp_mqtt_client_config_t SailtrackModule::mqttConfig;
esp_mqtt_client_handle_t SailtrackModule::mqttClient;
bool SailtrackModule::mqttConnected;
int SailtrackModule::publishedMessagesCount;

void SailtrackModule::configWifi(const char * ssid, const char * password, IPAddress gateway, IPAddress subnet) {
    wifiConfig.ssid = ssid;
    wifiConfig.password = password;
    wifiConfig.gateway = gateway;
    wifiConfig.subnet = subnet;
}

void SailtrackModule::configMqtt(IPAddress host, int port, const char * username, const char * password) {
    mqttConfig.host = strdup(host.toString().c_str());
    mqttConfig.port = port;
    mqttConfig.username = username;
    mqttConfig.password = password;
}

void SailtrackModule::begin(const char * name, IPAddress ip) {
    SailtrackModule::name = name;
    beginLogging();
    beginWifi(ip);
    beginOTA();
    beginMqtt();
}

void SailtrackModule::beginLogging() {
    Serial.begin(115200);
    Serial.println();
    esp_log_set_vprintf(m_vprintf);
    esp_log_level_set(LOG_TAG, ESP_LOG_INFO);
}

void SailtrackModule::beginWifi(IPAddress ip) {
    if (!wifiConfig.ssid) wifiConfig.ssid = WIFI_DEFAULT_SSID;
    if (!wifiConfig.password) wifiConfig.password = WIFI_DEFAULT_PASSWORD;
    if (!wifiConfig.gateway) wifiConfig.gateway = WIFI_DEFAULT_GATEWAY;
    if (!wifiConfig.subnet) wifiConfig.subnet = WIFI_DEFAULT_SUBNET;
    char hostname[30] = "sailtrack-";
    wifiConfig.hostname = strdup(strcat(hostname, name));
    wifiConfig.ip = ip;

    WiFi.mode(WIFI_STA);
    WiFi.setHostname(wifiConfig.hostname);
    WiFi.config(wifiConfig.ip, wifiConfig.gateway, wifiConfig.subnet);
    WiFi.begin(wifiConfig.ssid, wifiConfig.password);

    if (callbacks) callbacks->onWifiConnectionBegin();
    
    ESP_LOGI(LOG_TAG, "Connecting to '%s'...", wifiConfig.ssid);

    for (int i = 0; WiFi.status() != WL_CONNECTED && i < WIFI_CONNECTION_TIMEOUT_MS / 500 ; i++)
        delay(500);

    if (WiFi.status() != WL_CONNECTED) {
        ESP_LOGI(LOG_TAG, "Impossible to connect to '%s'", wifiConfig.ssid);
        ESP_LOGI(LOG_TAG, "Going to deep sleep, goodnight...");
        ESP.deepSleep(WIFI_SLEEP_DURATION_US);
    }

    if (callbacks) callbacks->onWifiConnectionResult(WiFi.status());

    ESP_LOGI(LOG_TAG, "Successfully connected to '%s'!", wifiConfig.ssid);

    WiFi.onEvent([](WiFiEvent_t event) {
        ESP_LOGE(LOG_TAG, "Lost connection to '%s'", wifiConfig.ssid);
        if (callbacks) callbacks->onWifiDisconnected();
        ESP_LOGE(LOG_TAG, "Rebooting...");
        ESP.restart();
    }, SYSTEM_EVENT_STA_DISCONNECTED);
}

void SailtrackModule::beginOTA() {
    ArduinoOTA
        .onStart([]() {
            if (ArduinoOTA.getCommand() == U_FLASH) ESP_LOGI(LOG_TAG, "Start updating sketch...");
            else ESP_LOGI(LOG_TAG, "Start updating filesystem...");
            esp_mqtt_client_stop(mqttClient);
        })
        .onEnd([]() {
            ESP_LOGI(LOG_TAG, "Update successfully completed!");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            ESP_LOGV(LOG_TAG, "Progress: %u", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            if (error == OTA_AUTH_ERROR) ESP_LOGE(LOG_TAG, "Error[%u]: Auth Failed", error);
            else if (error == OTA_BEGIN_ERROR) ESP_LOGE(LOG_TAG, "Error[%u]: Begin Failed", error);
            else if (error == OTA_CONNECT_ERROR) ESP_LOGE(LOG_TAG, "Error[%u]: Connect Failed", error);
            else if (error == OTA_RECEIVE_ERROR) ESP_LOGE(LOG_TAG, "Error[%u]: Receive Failed", error);
            else if (error == OTA_END_ERROR) ESP_LOGE(LOG_TAG, "Error[%u]: End Failed", error);
        });
    ArduinoOTA.setHostname(wifiConfig.hostname);
    ArduinoOTA.begin();
    xTaskCreate(otaTask, "ota_task", TASK_MEDIUM_STACK_SIZE, NULL, TASK_MEDIUM_PRIORITY, NULL);
}

void SailtrackModule::beginMqtt() {
    if (!mqttConfig.host) mqttConfig.host = strdup(MQTT_DEFAULT_HOST.toString().c_str());
    if (!mqttConfig.port) mqttConfig.port = MQTT_DEFAULT_PORT;
    if (!mqttConfig.username) mqttConfig.username = MQTT_DEFAULT_USERNAME;
    if (!mqttConfig.password) mqttConfig.password = MQTT_DEFAULT_PASSWORD;
    mqttConfig.client_id = wifiConfig.hostname;

    mqttClient = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_start(mqttClient);
    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_CONNECTED, mqttEventHandler, NULL);

    ESP_LOGI(LOG_TAG, "Connecting to 'mqtt://%s@%s:%d'...", mqttConfig.username, mqttConfig.host, mqttConfig.port);

    for (int i = 0; !mqttConnected && i < MQTT_CONNECTION_TIMEOUT_MS / 500; i++)
        delay(500);

    if (!mqttConnected) {
        ESP_LOGE(LOG_TAG, "Impossible to connect to 'mqtt://%s@%s:%d'", mqttConfig.username, mqttConfig.host, mqttConfig.port);
        ESP_LOGE(LOG_TAG, "Rebooting...");
        ESP.restart();
    }

    ESP_LOGI(LOG_TAG, "Successfully connected to 'mqtt://%s@%s:%d'!", mqttConfig.username, mqttConfig.host, mqttConfig.port);

    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_DATA, mqttEventHandler, NULL);
    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_DISCONNECTED, mqttEventHandler, NULL);
    esp_mqtt_client_register_event(mqttClient, MQTT_EVENT_PUBLISHED, mqttEventHandler, NULL);
    xTaskCreate(logTask, "log_task", TASK_MEDIUM_STACK_SIZE, NULL, TASK_LOW_PRIORITY, NULL);
}

void SailtrackModule::mqttEventHandler(void * handlerArgs, esp_event_base_t base, int32_t eventId, void * eventData) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)eventData;
    switch((esp_mqtt_event_id_t)eventId) {
        case MQTT_EVENT_CONNECTED:
            if (callbacks) callbacks->onMqttConnected();
            mqttConnected = true;
            break;
        case MQTT_EVENT_DATA:
            char topic[20];
            char message[500];
            sprintf(topic, "%.*s", event->topic_len, event->topic);
            sprintf(message, "%.*s", event->data_len, event->data);
            if (callbacks) callbacks->onMqttMessage(topic, message);
            break;
        case MQTT_EVENT_DISCONNECTED:
            if (callbacks) callbacks->onMqttDisconnected();
            mqttConnected = false;
            break;
        case MQTT_EVENT_PUBLISHED:
            publishedMessagesCount++;
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
        publish(topic, payload);
        delay(STATUS_PUBLISH_PERIOD_MS);
    }
}

void SailtrackModule::logTask(void * pvArguments) {
    while(true) {
        ESP_LOGI(LOG_TAG, "Published messages: %d", publishedMessagesCount);
        delay(LOG_PUBLISH_PERIOD_MS);
    }
}

void SailtrackModule::otaTask(void * pvArguments) {
    while (true) {
        ArduinoOTA.handle();
        delay(OTA_HANDLE_PERIOD_MS);
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
        publish(topic, payload);
    }
    return vprintf(format, args);
}

int SailtrackModule::publish(const char * topic, DynamicJsonDocument payload) {
    payload["measurement"] = strrchr(strdup(topic), '/') + 1;
    char output[MQTT_OUTPUT_BUFFER_SIZE];
    serializeJson(payload, output);
    return esp_mqtt_client_publish(mqttClient, topic, output, 0, 1, 0);
}

int SailtrackModule::subscribe(const char * topic) {
    return esp_mqtt_client_subscribe(mqttClient, topic, 1);
}
