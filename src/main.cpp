#include <Arduino.h>
#include <ArduinoJson.h>

#include "SailtrackModule.h"

int counter = 0;

void publishTask(void * pvArguments) {
	while(true) {
		DynamicJsonDocument doc(JSON_OBJECT_SIZE(1));
		doc["test_counter"] = counter++;
		STModule.publish("sensor/test0", "test0", doc);
		delay(1000);
	}
}

static void onMessage(void * handlerArgs, esp_event_base_t base, int32_t eventId, void * eventData) {
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) eventData;
	Serial.printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    Serial.printf("DATA=%.*s\r\n", event->data_len, event->data);
}

void setup() {
	STModule.init("test");
	STModule.registerEvent(MQTT_EVENT_DATA, onMessage, NULL);
	STModule.subscribe("sensor/test1");
	xTaskCreate(publishTask, "publish_task", 10000, NULL, 1, NULL);
}

void loop() {
	
}
