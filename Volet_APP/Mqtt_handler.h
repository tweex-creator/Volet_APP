#pragma once
#include <WiFi.h>
#include <PubSubClient.h>
#include "Volet.h"
#include "Capteurs.h"
class Mqtt_handler
{
public:
	Mqtt_handler(const char* mqtt_server, Volet* volet, Capteurs* capteurs);
	~Mqtt_handler();
	void loop();
	void callback(char* topic, byte* message, unsigned int length);
private:
	const char* mqtt_server;
	char* id;
	WiFiClient espClient;
	PubSubClient client;

	//reference
	Volet* volet_ptr;
	Capteurs* capteurs_ptr;

	void reconnect();
	unsigned long lastReconnectAttempt;

	void sendRecap();
	unsigned long lastSendRecap;

	void subscribe();
};

