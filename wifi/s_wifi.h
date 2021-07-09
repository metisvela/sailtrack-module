#ifndef SWIFI
#define SWIFI

#include <Arduino.h>
#include <WiFi.h>

#define NB_TRYWIFI 10           // Number of wifi connection tries before going to sleep
#define SLEEP_DURATION 60 * 1e6 // Sleep duration [us]

#define SSID "SailTrack-Net" // WiFi credentials
#define PSW "sailtracknet"

#define FRQ 100

#define HOSTNAME "sensor-unknown"


void setup_wifi();
void monitor_wifi(void *param);


#endif