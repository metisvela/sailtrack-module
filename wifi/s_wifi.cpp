#include <wifi/s_wifi.h>

void setup_wifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(HOSTNAME);
    WiFi.begin(SSID, PSW);

    esp_sleep_enable_timer_wakeup(SLEEP_DURATION);

    int _try = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        _try++;
        Serial.print(".");
        if (_try >= NB_TRYWIFI)
        {
            Serial.println("Impossible to connect WiFi network, going to deep sleep, goodnight!");
            esp_deep_sleep_start();
        }
    }
    Serial.println("WiFi Connected!");

    xTaskCreate(monitor_wifi, "monitor_wifi", 1000, NULL, 2, NULL);
}

void monitor_wifi(void *param)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = FRQ / portTICK_RATE_MS;
    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Lost Connection to WiFi, going to deep sleep, goodnight!");
            esp_deep_sleep_start();
        }
    }
}