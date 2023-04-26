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
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);
const char* ap_ssid = "volet";
const char* ap_password = "voletApp12345";

Volet* link_volet = nullptr;
Capteurs* link_capteurs = nullptr;
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

	WiFi.softAP(ap_ssid, ap_password);

	Serial.print("Access Point \"");
	Serial.print(ap_ssid);
	Serial.println("\" started");
	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send_P(200, "text/html", "welcome");
		});

	server.on("/ouvrir", HTTP_GET, [](AsyncWebServerRequest* request) {
		int value = request->getParam("val")->value().toInt();
		Serial.print("value received: ");
		Serial.println(value);
		link_volet->setPosBD(value);
		link_volet->setPosBG(value);
		request->send_P(200, "text/html", "deplacement en cours...");
		});

	server.on("/po", HTTP_GET, [](AsyncWebServerRequest* request) {
		link_volet->priseOrigine();
		request->send_P(200, "text/html", "Prise origine en cours...");
		});

	server.on("/calibration", HTTP_GET, [](AsyncWebServerRequest* request) {
		link_volet->calibrate();
		request->send_P(200, "text/html", "calibration en cours...");
		});

	server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) {

		String text = "<html>  <head>    <title>HTML in 10 Simple Steps or Less</title>    <meta http-equiv=\"refresh\" content=\"1\" /> </head> <body>";
			"valeurs des capteurs: <br>";
		text += "	Interieur: <br>";
		text += "humidite: " + String(link_capteurs->getHumInt()) + "<br>";
		text += "Temperature: " + String(link_capteurs->getTempInt()) + "<br> <br>";

		text += "	Exterieur: <br>";
		text += "humidite: " + String(link_capteurs->getHumExt()) + "<br>";
		text += "Temperature: " + String(link_capteurs->getTempExt()) + "<br>";		
		text += "Luminosite: " + String(link_capteurs->getLuxExt()) + "<br> <br>";	

		text += "	Position des volets:<br>";
		text += "battant gauche: " + String(link_volet->getPosBG()) + "<br>";		
		text += "battant droit: " + String(link_volet->getPosBD()) + "<br><br>";

		text += "	Position desiree:<br>";
		text += "battant gauche: " + String(link_volet->getTargetBG()) + "<br>";
		text += "battant droit: " + String(link_volet->getTargetBD()) + "<br><br>";

		text += "	Position virtuelle:<br>";
		text += "battant gauche: " + String(link_volet->getRealTargetBG()) + "<br>";
		text += "battant droit: " + String(link_volet->getRealTargetBD()) + "<br>";
		text += "  </body></html>";

		request->send_P(200, "text/html", text.c_str());
		});
	server.begin();

}


void loop() {
	Volet* volet = new Volet(0x44, 0x40);
	link_volet = volet;
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

	Capteurs capteurs(25, 26, 27, 14);
	link_capteurs = &capteurs;
	const char* mqtt_server = "192.168.43.249";
	Mqtt_handler mqtt(mqtt_server, volet, &capteurs);
	while (true) {
		mqtt.loop();
		volet->loop();
	}

}
