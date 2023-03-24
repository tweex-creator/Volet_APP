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

#include <FS.h>
#include <LittleFS.h>



void setup() {
	Serial.begin(115200);

	Serial.println("Starting...");
	Serial.println(F("Inizializing FS..."));
	if (!LittleFS.begin()) {
		Serial.println("An Error has occurred while mounting LittleFS");
		return;
	}

	
}

void loop() {
	Volet volet(0x44, 0x40);
	configBattant battant_droit_conf;
	configBattant battant_gauche_conf;
	battant_droit_conf.battantType = 1;
	battant_droit_conf.pont_H_pinFermeture = 2;
	battant_droit_conf.pont_H_pinOuverture = 4;

	battant_gauche_conf.battantType = 0;
	battant_gauche_conf.pont_H_pinFermeture = 32;
	battant_gauche_conf.pont_H_pinOuverture = 33;

	volet.config(battant_gauche_conf, battant_droit_conf);

	//volet.init_calibration();
	volet.calibrate_manual(15815,15755,15492,15554);

	//volet.setPosBD(0.0);
	//volet.setPosBG(0.0);
	while (true) {
		volet.loop();

		if (Serial.available()) {
			float pos_serie = Serial.parseFloat();
			if (pos_serie >= 0 && pos_serie <= 100) {
				volet.setPosBD(pos_serie);
				volet.setPosBG(pos_serie);

			}
		}
	}

}
