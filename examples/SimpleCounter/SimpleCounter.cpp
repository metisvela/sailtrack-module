#include <Arduino.h>

#include <SailtrackModule.h>

SailtrackModule STM;

class TestCallbacks: public SailtrackModuleCallbacks {
	void onWifiConnectionBegin() {}
	void onWifiConnectionResult(wl_status_t status) {}
	void onWifiDisconnected() {}
	void onMqttConnectionBegin() {}
	void onMqttConnectionResult(bool connected) {}
	void onMqttDisconnected() {}

	DynamicJsonDocument getStatus() {
		// Read module status
		DynamicJsonDocument doc(500);
		return doc;
	}

	void onMqttMessage(const char * topic, const char * message) {
		Serial.println("NEW MESSAGE");
		Serial.printf("Topic: %s, Message: %s\n", topic, message);
	}
};

int counter = 0;

void setup() {
	STM.begin("counter", IPAddress(192, 168, 42, 100), new TestCallbacks());
	STM.subscribe("sensor/counter0");
}

void loop() {
	DynamicJsonDocument data(100);
	data["count"] = counter++;
	STM.publish("sensor/counter0", data);
	delay(1000);
}
