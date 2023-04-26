/*
 Name:		Volet_APP.ino
 Created:	09/02/2023 08:36:44
 Author:	Hervé Eruam
*/

// the setup function runs once when you press reset or power the board



#include <AsyncUDP.h>
#include "Volet.h"
#include "Capteurs.h"
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "Mqtt_handler.h"
#include "webServer.h"


void setup() {
	Serial.begin(115200);
	WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
	// it is a good practice to make sure your code sets wifi mode how you want it.

	// put your setup code here, to run once:
	Serial.begin(115200);

	//WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
	WiFiManager wm;

	// reset settings - wipe stored credentials for testing
	// these are stored by the esp library
	//wm.resetSettings();

	// Automatically connect using saved credentials,
	// if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
	// if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
	// then goes into a blocking loop awaiting configuration and will return success result

	bool res;
	// res = wm.autoConnect(); // auto generated AP name from chipid
	// res = wm.autoConnect("AutoConnectAP"); // anonymous ap
	res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap

	if (!res) {
		Serial.println("Failed to connect");
		// ESP.restart();
	}
	else {
		//if you get here you have connected to the WiFi    
		Serial.println("connected...yeey :)");
	}
	
	
	Serial.println("Starting...");
}

void loop() {
	Volet* volet = new Volet(0x44, 0x40);
	configBattant battant_droit_conf;
	configBattant battant_gauche_conf;
	battant_droit_conf.battantType = 1;
	battant_droit_conf.pont_H_pinFermeture = 2;
	battant_droit_conf.pont_H_pinOuverture = 4;

	battant_gauche_conf.battantType = 0;
	battant_gauche_conf.pont_H_pinFermeture = 32;
	battant_gauche_conf.pont_H_pinOuverture = 33;

	volet->config(battant_gauche_conf, battant_droit_conf);

	//volet->calibrate();
	volet->calibrate_manual(20196, 20502, 20196, 20196);

	unsigned long perf = millis();
	unsigned long perf2;
	Capteurs capteurs(25, 26, 27, 14);
	const char* mqtt_server = "192.168.43.249";
	Mqtt_handler mqtt(mqtt_server, volet, &capteurs);
	while (true) {
		mqtt.loop();
		volet->loop();
	}

}
