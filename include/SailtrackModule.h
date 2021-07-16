#ifndef SAILTRACK_MODULE_H
#define SAILTRACK_MODULE_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "SailtrackConfig.h"

class SailtrackModule {
    private:
        static PubSubClient * mqtt;
        static void initWifi(const char * hostname);
        static void initMqtt(const char * name);
    public:
        static void init(const char * name);
        static void loop();
};

#endif
