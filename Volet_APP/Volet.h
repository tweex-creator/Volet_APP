#pragma once
#include "Battant.h"
#include <FS.h>
#include <LittleFS.h>

class Volet
{
private:
	Battant battantDroit;
	Battant battantGauche;

public:
	Volet(uint8_t addr_i2c_droit, uint8_t addr_i2c_gauche);
	void config(configBattant bg, configBattant bd);
	void calibrate();
	bool load_calibration();
	void calibrate_manual(long time_close_bd, long time_open_bd, long time_close_bg, long time_open_bg);

	void setPosBG(float pos);
	void setPosBD(float pos);
	void loop();

};

