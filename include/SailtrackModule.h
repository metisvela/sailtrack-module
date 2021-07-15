#ifndef SAILTRACK_MODULE_H
#define SAILTRACK_MODULE_H

#include <Arduino.h>
#include <WiFi.h>

#include "config.h"

class SailtrackModule {
    private:
        void initWifi(const char * hostname);
    public:
        SailtrackModule(const char * name);
};

#endif
