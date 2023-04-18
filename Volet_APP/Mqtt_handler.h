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
	char* id;
	WiFiClient espClient;
	PubSubClient client;

	void reconnect();
	unsigned long lastReconnectAttempt;

	void sendRecap();
	unsigned long lastSendRecap;

	void subscribe();
};

