<p align="center">
  <img src="https://raw.githubusercontent.com/metis-vela-unipd/sailtrack-docs/main/Assets/SailTrack%20Logo.svg" width="180">
</p>

<p align="center">
  <img src="https://img.shields.io/github/license/metis-vela-unipd/sailtrack-module" />
  <img src="https://img.shields.io/github/v/release/metis-vela-unipd/sailtrack-module" />
  <img src="https://img.shields.io/github/workflow/status/metis-vela-unipd/sailtrack-module/Publish%20Release" />
</p>


# SailTrack Module

SailTrack Module is the base library required by all the SailTrack's ESP32-based modules. To learn more about the SailTrack project, please visit the [documentation repository](https://github.com/metis-vela-unipd/sailtrack-docs).

The SailTrack Module library carries out the minimum tasks required by every module of the SailTrack system, such as:

- The WiFi connection routine.
- The connection to the MQTT broker.
- The publication of status messages.

Moreover, the library offers all the required methods to send correctly formatted messages and to subscribe to network topics.


## Installation

The SailTrack Module library can be installed using [PlatformIO](https://platformio.org) by simply adding it in the `platformio.ini` file, under the `lib_deps` section, as in the following example:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = metisvela/SailtrackModule@^1.7.1
```

## Usage

A bare minimum example of the library can be found below. For further information, please refer to the methods documentation and the examples that can be found in the `examples` folder.

```c++
#include <Arduino.h>
#include <SailtrackModule.h>

SailtrackModule stm;

class ModuleCallbacks: public SailtrackModuleCallbacks {
    void onLogMessage() {
        // Called when a new log message is being displayed to the serial console.
        // Add here any extra log information.
    }

    void onStatusMessage(JsonObject status) {
        // Called when a new status message is being published to the MQTT network.
        // Add here any extra status information (e.g. battery voltage) by adding it to the `status` object.
    }

    void onMqttMessage(const char * topic, JsonObjectConst payload) {
        // Called when a message is being published to any of the subscribed MQTT topics.
        // In this example, this method is called every time a new message is being published under the 
        // `sensor/example2` topic.
        // Handle here received messages from the network by reading the `payload` object and the `topic` string.
    }
};

void setup() {
    // Initialize the module. This method must be always called.
    stm.begin("example", IPAddress(192, 168, 1, 100), new ModuleCallbacks());
    // Subscribe to the `sensor/example2` topic. This means that the `onMqttMessage(...)` method above will be 
    // called every time a new message is being published under `sensor/example2`.
    stm.subscribe("sensor/example2");
}

void loop() {
    // Publish the "Hello, World!" message under the `sensor/example` topic every second by creating the JSON 
    // object containing the message and using the `publish(...)` method.
    StaticJsonDocument<STM_JSON_DOCUMENT_SMALL_SIZE> doc;
    doc["example"] = "Hello, World!";
    stm.publish("sensor/example", doc.as<JsonObjectConst>());
    delay(1000);
}
```

## Configuration

It is possible to change some settings of the library by overriding one or more of the following internal defines:
 - `STM_NOTIFICATION_LED_PIN` (integer): The pin to which the notification LED is connected. Defaults to the board's builtin LED pin, if present.
 - `STM_NOTIFICATION_LED_ON_STATE` (`LOW` or `HIGH`): The logic state that corresponds to the ON state of the notification LED. Deafults to `HIGH` (=1).
 - `STM_WIFI_AP_SSID` (string): The name of the WiFi network to which the module will try to connect at startup. Defaults to "SailTrack-CoreNet" (the SSID of the WiFi network created by SailTrack Core).
 - `STM_WIFI_AP_PASSWORD` (string): The password of the WiFi network to which the module will try to connect at startup. Defaults to "sailtracknet" (the deafult password of the WiFi network created by SailTrack Core).
 - `STM_WIFI_GATEWAY_ADDR` (string): The IP address of the gateway (i.e. the router) to which the module will try to connect at startup. Defaults to "192.168.42.1" (the default IP address of SailTrack Core).
 - `STM_WIFI_SUBNET` (string): The subnet mask of the network to which the module will try to connect at startup. Defaults to "255.255.255.0" (the default subnet mask of the network created by SailTrack Core).
 - `STM_MQTT_HOST_ADDR` (string): The IP address of the host running the MQTT broker to which the module will try to connect at startup. Defaults to "192.168.42.1" (the default IP address of SailTrack Core, that is also running the MQTT broker).
 - `STM_MQTT_PORT` (integer): The MQTT port number to which the module will try to connect at startup. Defaults to 1883 (the default MQTT port).
 - `STM_MQTT_USERNAME` (string): The username used to authenticate to the MQTT broker. Defaults to "mosquitto" (the default broker's username).
 - `STM_MQTT_PASSWORD` (string): The password used to authenticate to the MQTT broker. Defaults to "sailtrack" (the default broker's password).
 
To confifure the library, you can add the overridden defines in the `platformio.ini` file, under the `build_flags` section, as in the following example (double quotes must be escaped):
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = metisvela/SailtrackModule@^1.7.1
build_flags = 
  -D STM_WIFI_AP_SSID=\"My-WiFi\"
  -D STM_WIFI_AP_PASSWORD=\"My-Password\"
```

## Contributing

Pull requests are welcome. For major changes, please [open an issue](https://github.com/metis-vela-unipd/sailtrack-core/issues/new) first to discuss what you would like to change.

If you are a contributor and you don't have access to SailTrack Core, you can use this procedure to emulate it:
 1. [Install Docker](https://docs.docker.com/get-docker/).
 2. Run the following command:
 ```
 docker run -it -p 1883:1883 eclipse-mosquitto:<version> mosquitto -c /mosquitto-no-auth.conf
 ```
 3. Add the following `build_flags` to your `platformio.ini` file, properly setting the options:
 ```ini
build_flags = 
  -D STM_WIFI_AP_SSID=\"<your-wifi-name>\"
  -D STM_WIFI_AP_PASSWORD=\"<your-wifi-password>\"
  -D STM_WIFI_GATEWAY_ADDR=\"<your-router-ip>\"
  -D STM_MQTT_HOST_ADDR=\"<your-pc-ip>\"
 ```
 4. Set the module IP in the `begin(...)` method according to your network.
 5. Check the MQTT traffic using an application like [MQTT Explorer](http://mqtt-explorer.com).

## License

Copyright © 2022, [Métis Vela Unipd](https://github.com/metis-vela-unipd). SailTrack Core is available under the [GPL-3.0 license](https://www.gnu.org/licenses/gpl-3.0.en.html). See the LICENSE file for more info. 
