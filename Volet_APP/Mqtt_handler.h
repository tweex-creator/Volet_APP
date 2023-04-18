#pragma once
#include <WiFi.h>
#include <PubSubClient.h>

class Mqtt_handler
{
public:
	Mqtt_handler(const char* mqtt_server);
	~Mqtt_handler();
	void loop();
	void callback(char* topic, byte* message, unsigned int length);
private:
	const char* mqtt_server;
	WiFiClient espClient;
	PubSubClient client;

	void reconnect();
	unsigned long lastReconnectAttempt;


	void subscribe();
};

