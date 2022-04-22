#include <Arduino.h>
#include <SailtrackModule.h>

SailtrackModule stm;

class ModuleCallbacks: public SailtrackModuleCallbacks {
	void onLogMessage() {
		log_i("Extra log entry");
		log_printf("\n");
	}

	void onStatusMessage(JsonObject status) {
		status["extra"] = "extraStatus";
	}

	void onMqttMessage(const char * topic, const char * message) {
		log_i("New message! Topic: %s, Message: %s", topic, message);
	}
};

int counter = 0;

void setup() {
	stm.begin("counter", IPAddress(192, 168, 42, 100), new ModuleCallbacks());
	stm.subscribe("sensor/counter0");
}

void loop() {
	StaticJsonDocument<128> doc;
	doc["count"] = counter++;
	stm.publish("sensor/counter0", doc.as<JsonObjectConst>());
	delay(1000);
}
