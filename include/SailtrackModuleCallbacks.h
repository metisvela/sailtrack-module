#ifndef SAILTRACK_MODULE_CALLBACKS_H
#define SAILTRACK_MODULE_CALLBACKS_H

/**
 * Class containing the callbacks used by the library in order to notify and request values from the main application.
 */ 
class SailtrackModuleCallbacks {
    public:
        /**
         * Called when a new MQTT message has been published to one of the subscribed topics.
         * 
         * @param topic The topic the message has been published to.
         * @param message The received message.
         */
        virtual void onMqttMessage(const char * topic, const char * message) {}

        /**
         * Called when a new log entry is being sent through the serial port.
         */
        virtual void onLogMessage() {}

        /**
         * Called when a new status message is being sent through MQTT.
         * 
         * @param status The JsonObject with the status of the module, you can modify it to include extra status
         *      information of the module.
         */
        virtual void onStatusMessage(JsonObject status) {}
};

#endif
