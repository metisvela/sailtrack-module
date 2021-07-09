#include <mqtt/s_mqtt.h>

esp_mqtt_client_handle_t setup_mqtt()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .username = USERNAME,
        .host = HOST,
        .password = PASSWORD,
        .port = PORT
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);

    // xTaskCreate(monitor_mqtt, "monitor_mqtt", 10000, client, 1, NULL)
    return client;
}

void monitor_mqtt(void *param)
{

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = FRQ / portTICK_RATE_MS;

    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Serial.println("TEST");
    }
}
