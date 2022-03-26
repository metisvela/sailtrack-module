#ifndef SAILTRACK_MODULE_CALLBACKS_H
#define SAILTRACK_MODULE_CALLBACKS_H

/**
 * Class containing the callbacks used by the library in order to notify and request values from the main application.
 */ 
class SailtrackModuleCallbacks {
    public:

        /**
         * Called when the connection to the WiFi network begins.
         */ 
        virtual void onWifiConnectionBegin() {}

        /**
         * Called when the connection to the WiFi network ends, either successfully or not.
         * 
         * @param status The resulted status of the connection.
         */
        virtual void onWifiConnectionResult(wl_status_t status) {}

        /**
         * Called when the connection to the WiFi network is lost. Note that once the callback returns the ESP is 
         * rebooted, therefore remember not to start any non-blocking task from this callback, as it will be killed 
         * immediately.
         */
        virtual void onWifiDisconnected() {}

        /**
         * Called when the connection to the MQTT broker begins.
         */ 
        virtual void onMqttConnectionBegin() {}

        /**
         * Called when the connection to the MQTT broker ends, either successfully or not.
         * 
         * @param connected The resulted status of the connection.
         */ 
        virtual void onMqttConnectionResult(bool connected) {}

        /**
         * Called when the connection to the MQTT broker is lost. Note that once the callback returns the ESP is
         * rebooted, therefore remember not to start any non-blocking task from this callback, as it will be killed
         * immediately.
         */
        virtual void onMqttDisconnected() {}

        /**
         * Called when a new MQTT message is published to one of the subscribed topics.
         * 
         * @param topic The topic the message has been published to.
         * @param message The received message.
         */
        virtual void onMqttMessage(const char * topic, const char * message) {}

        /**
         * Called when the library needs the status of the module (e.g. to publish it to MQTT).
         * 
         * @return The status of the module, as a JSON document.
         */
        virtual DynamicJsonDocument * getStatus() { return NULL; }
};

#endif
