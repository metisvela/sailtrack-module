#ifndef SAILTRACK_CONFIG_H
#define SAILTRACK_CONFIG_H

#define WIFI_SSID "SailTrack-Net"
#define WIFI_PSW "sailtracknet"
#define WIFI_SLEEP_DURATION 60 * 1e6
#define WIFI_GATEWAY IPAddress(192, 168, 42, 1)
#define WIFI_SUBNET IPAddress(255, 255, 255, 0)

#define MQTT_SERVER_IP "192.168.42.1"
#define MQTT_PORT 1883
#define MQTT_USERNAME "mosquitto"
#define MQTT_PASSWORD "dietpi"

#endif
