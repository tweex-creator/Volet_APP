#include "Mqtt_handler.h"

Mqtt_handler* Mqtt_ptr_callback = nullptr;

void callback_ext(char* topic, byte* message, unsigned int length) {
    if (Mqtt_ptr_callback == nullptr) {
        return;
    }
    Mqtt_ptr_callback->callback(topic, message, length);
}



Mqtt_handler::Mqtt_handler(const char* mqtt_server):client(espClient)
{
	this->mqtt_server = mqtt_server;
	client.setServer(mqtt_server, 1883);
    client.setCallback(callback_ext);
    lastReconnectAttempt = -1; // -1 allows the first attempt to be made immediately
}

Mqtt_handler::~Mqtt_handler()
{
    this->mqtt_server = nullptr;
}

void Mqtt_handler::loop()
{
    if (!client.connected()) {
		this->reconnect();
	}
	client.loop();
}

void Mqtt_handler::reconnect()
{
    if (this->lastReconnectAttempt > 0 && millis() - this->lastReconnectAttempt < 5000) {
		return;
	}
    this->lastReconnectAttempt = millis();
    // Loop until we're reconnected
    if (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32Client")) {
            Serial.println("connected");
            // Subscribe
            this->subscribe();
        }
        else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
        }
    }
}

void Mqtt_handler::subscribe()
{
    client.subscribe("topic1");
    client.subscribe("topic2");
    //...

}

void Mqtt_handler::callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    // Feel free to add more if statements to control more GPIOs with MQTT

    // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
    // Changes the output state according to the message
    if (String(topic) == "esp32/output") {
        Serial.print("Changing output to ");
        if (messageTemp == "on") {
            Serial.println("on");
        }
        else if (messageTemp == "off") {
            Serial.println("off");
        }
    }
}