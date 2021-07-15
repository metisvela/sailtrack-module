#include <Arduino.h>

#include "SailtrackModule.h"

void publishTest() {
    SailtrackModule::mqtt->publish("Test", "ciao bro");
}

void setup() {
	SailtrackModule::init("test-module");
    xTask
  SailtrackModule::init("test-module");
}

void loop() {
	SailtrackModule::loop();
}
