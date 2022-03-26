#define USE_ESP_IDF_LOG

#include <Arduino.h>

#include <SailtrackModule.h>

SailtrackModule STM;

static const char * LOG_TAG = "SAILTRACK_COUNTER";

class TestCallbacks: public SailtrackModuleCallbacks {
	void onWifiConnectionBegin() {}
	void onWifiConnectionResult(wl_status_t status) {}
	void onWifiDisconnected() {}
	void onMqttConnectionBegin() {}
	void onMqttConnectionResult(bool connected) {}
	void onMqttDisconnected() {}

	DynamicJsonDocument * getStatus() { return NULL; }

	void onMqttMessage(const char * topic, const char * message) {
		ESP_LOGI(LOG_TAG, "New message! Topic: %s, Message: %s", topic, message);
	}
};

int counter = 0;

void setup() {
	STM.begin("counter", IPAddress(192, 168, 42, 100), new TestCallbacks());
	STM.subscribe("sensor/counter0");
	esp_log_level_set(LOG_TAG, ESP_LOG_INFO);
}

void loop() {
	DynamicJsonDocument data(100);
	data["count"] = counter++;
	STM.publish("sensor/counter0", &data);
	delay(1000);
}
