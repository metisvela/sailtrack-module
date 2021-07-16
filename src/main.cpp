#include <Arduino.h>

#include "SailtrackModule.h"

int prev_millis = millis();

void testTask(void * pvParameters) {
	while (true) {
		SailtrackModule::publish("test_topic_task", "bella fra");
		Serial.println("Test publish (task)");
		delay(1000);
	}
}

void setup() {
	SailtrackModule::init("test-module");
	xTaskCreate(&testTask, "test_task", 1024, NULL, 1, NULL);
}

void loop() {
	SailtrackModule::loop();
	if (millis() - prev_millis >= 5000) {
		prev_millis = millis();
		Serial.println("Test publish (no task)");
		SailtrackModule::publish("test_topic_no_task", "bella fra");
	}
}
