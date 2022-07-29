<p align="center">
  <img src="https://raw.githubusercontent.com/metis-vela-unipd/sailtrack-docs/main/Assets/SailTrack%20Logo.png" width="180">
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

## Contributing

Pull requests are welcome. For major changes, please [open an issue](https://github.com/metis-vela-unipd/sailtrack-core/issues/new) first to discuss what you would like to change.

## License

Copyright © 2022, [Métis Vela Unipd](https://github.com/metis-vela-unipd). SailTrack Core is available under the [GPL-3.0 license](https://www.gnu.org/licenses/gpl-3.0.en.html). See the LICENSE file for more info. 
