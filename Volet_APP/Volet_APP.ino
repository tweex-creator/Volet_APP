/*
 Name:		Volet_APP.ino
 Created:	09/02/2023 08:36:44
 Author:	Hervé Eruam
*/

// the setup function runs once when you press reset or power the board

#include <Preferences.h>
#include <SD_MMC.h>
#include <sd_defines.h>
#include <Adafruit_INA219.h>
#include "Volet.h"
#include "Capteurs.h"

void setup() {
	Serial.begin(115200);

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

	//volet.setPosBD(0.0);
	//volet.setPosBG(0.0);
	unsigned long perf = millis();
	unsigned long perf2;
	Capteurs capteurs(25, 26, 27);
	while (true) {
		if (millis() - perf > 1000) {
			perf = millis();
			Serial.print("Temp int: ");
			Serial.println(capteurs.getTempInt());
			Serial.print("Hum int: ");
			Serial.println(capteurs.getHumInt());
			Serial.print("Temp ext: ");
			Serial.println(capteurs.getTempExt());
			Serial.print("Hum ext: ");
			Serial.println(capteurs.getHumExt());
			Serial.print("Lux ext: ");
			Serial.println(capteurs.getLuxExt());
		}

		//perf2 = millis() - perf;
		//Serial.print("perf: ");
		//Serial.println(perf2);
		//perf = millis();
		volet->loop();
		if (Serial.available()) {
			float pos_serie = Serial.parseFloat();
			if (pos_serie >= 0 && pos_serie <= 100) {
				volet->setPosBD(pos_serie);
				volet->setPosBG(pos_serie);

			}
		}
	}

}
