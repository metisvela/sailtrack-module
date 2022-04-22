#include <Arduino.h>
#include <SailtrackModule.h>

#define LOOP_TASK_RATE_HZ 1

SailtrackModule stm;

class ModuleCallbacks: public SailtrackModuleCallbacks {
	void onLogMessage() {
		log_i("Extra log entry");
		log_printf("\n");
	}

	void onStatusMessage(JsonObject status) {
		status["extra"] = "extraStatus";
	}

	void onMqttMessage(const char * topic, JsonObjectConst payload) {
		char message[STM_MQTT_DATA_BUFFER_SIZE];
		serializeJson(payload, message);
		log_i("New message! Topic: %s, Message: %s", topic, message);
	}
};

int counter = 0;

void setup() {
	stm.begin("counter", IPAddress(192, 168, 42, 100), new ModuleCallbacks());
	stm.subscribe("sensor/counter0");
}

void loop() {
	TickType_t lastWakeTime = xTaskGetTickCount();
	StaticJsonDocument<STM_JSON_DOCUMENT_SMALL_SIZE> doc;
	doc["count"] = counter++;
	stm.publish("sensor/counter0", doc.as<JsonObjectConst>());
	vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000 / LOOP_TASK_RATE_HZ));
}
