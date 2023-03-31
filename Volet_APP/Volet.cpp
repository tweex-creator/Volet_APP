#include "Volet.h"

//Configuration
Volet::Volet(uint8_t addr_i2c_droit, uint8_t addr_i2c_gauche):battantDroit(addr_i2c_droit), battantGauche(addr_i2c_gauche)
{
	volet_state = 0;
	config_done = 0;
	calibration_var.state = 0;
	origine_var.state = 0;
}

void Volet::config(configBattant bg, configBattant bd)
{
	battantDroit.config(bd);
	battantGauche.config(bg);
	battantDroit.setAutreBattant(&battantGauche);
	battantGauche.setAutreBattant(&battantDroit);
	config_done = 1;
}

bool Volet::setVoletState(int state)
{
	this->volet_state = state;
	return true;
}

//Calibration
void Volet::calibrate()
{
	if (this->setVoletState(-2)) {
		this->calibration_var.state = 10;
	}
}

bool Volet::load_calibration()
{
	File calibration_file = LittleFS.open(F("/calibration_data.txt"), "r");
	unsigned long time_close_bd;
	unsigned long time_open_bd;
	unsigned long time_close_bg;
	unsigned long time_open_bg;
	if (calibration_file) {
		time_close_bd = calibration_file.parseInt();
		time_open_bd = calibration_file.parseInt();
		time_close_bg = calibration_file.parseInt();
		time_open_bg = calibration_file.parseInt();
		calibrate_manual(time_close_bd, time_open_bd, time_close_bg, time_open_bd);

	}
	else {
		Serial.println("failed to read calibration data");
		return false;
	}
	return true;
	calibration_file.close();
}

void Volet::calibrate_manual(long time_close_bd, long time_open_bd, long time_close_bg, long time_open_bg)
{
	battantDroit.set_time_close(time_close_bd);
	battantDroit.set_time_open(time_open_bd);
	battantGauche.set_time_close(time_close_bg);
	battantGauche.set_time_open(time_open_bg);
	Serial.print("close battant droit: ");
	Serial.println(time_close_bd);
	Serial.print("open battant droit: ");
	Serial.println(time_open_bd);
	Serial.print("close battant gauche: ");
	Serial.println(time_close_bg);
	Serial.print("open battant gauche: ");
	Serial.println(time_open_bd);	
}

void Volet::calibration_loop()
{
	switch (this->calibration_var.state) {
	case 0:

		break;
	case 10:
		//On demande aux deux battants de se preparer pour une calibration
		battantGauche.init_calibration();
		battantDroit.init_calibration();
		this->calibration_var.state = 11;
		break;
	case 11:
		//On attends qu'ils soient tt les deux près
		if (battantGauche.calibration_Ready() && battantDroit.calibration_Ready()) {
			this->calibration_var.state = 20;
		}
		break;
	case 20:
		battantDroit.calibrateNextStep();
		this->calibration_var.state = 21;

		break;
	case 21:
		if (!battantDroit.calibration_inProgress()) this->calibration_var.state = 30;
		break;
	case 30:
		battantGauche.calibrateNextStep();
		this->calibration_var.state = 31;
		break;
	case 31:
		if (!battantGauche.calibration_inProgress()) {
			this->calibration_var.state = 0;
			this->priseOrigine();
		}
		break;
	default:
		this->calibration_var.state = -1;
		break;
	}
}


//Prise d'origine

void Volet::priseOrigine()
{
	if (this->setVoletState(-1)) {
		this->origine_var.state = 10;
	}
}
void Volet::priseOrigine_loop()
{
	switch (this->origine_var.state) {
	case 0:

		break;
	case 10:
		Serial.println("[VOLET] Start prise origine");
		//On demande aux deux battants de se preparer pour une calibration
		battantGauche.init_priseOrigine();
		battantDroit.init_priseOrigine();
		this->origine_var.state = 11;
		break;
	case 11:
		//On attends qu'ils soient tt les deux près
		if (battantGauche.priseOrigine_Ready() && battantDroit.priseOrigine_Ready()) {
			this->origine_var.state = 20;
		}
		break;
	case 20:
		battantDroit.priseOrigineNextStep();
		this->origine_var.state = 21;

		break;
	case 21:
		if (!battantDroit.priseOrigine_inProgress()) {
			this->origine_var.state = 22;
			this->origine_var.clock = millis();
		}
		break;
	case 22:
		if (millis() - this->origine_var.clock > 200) {
			this->origine_var.state = 30;
		}
		break;
	case 30:
		battantGauche.priseOrigineNextStep();
		this->origine_var.state = 31;
		break;
	case 31:
		if (!battantGauche.priseOrigine_inProgress()) {
			this->origine_var.state = 0;
			this->setVoletState(2); 
			Serial.println("[VOLET] Fin prise origine");

		}
		break;
	default:
		this->origine_var.state = -1;
		break;
	}
}


//Contôle public
void Volet::setPosBG(float pos)
{
	if(volet_state == 2) battantGauche.setTargetPosition(pos);
}
void Volet::setPosBD(float pos)
{
	if (volet_state == 2) battantDroit.setTargetPosition(pos);
}


//loop

void Volet::loop()
{
	int state_vol_debug = this->volet_state;
	switch (this->volet_state) {
	case -4:

		break;
	case -3:

		break;
	case -2:
		calibration_loop();
		break;
	case -1:
		priseOrigine_loop();
		break;
	case 0:
		if (config_done) this->setVoletState(1);
		break;
	case 1:
		this->priseOrigine();
		break;
	case 2:
		if (battantDroit.getState() == -3 || battantGauche.getState() == -3) {
			this->priseOrigine();
		}
		break;
	case 3:

		break;
	default:
		Serial.print("wrong state: ");
		Serial.println(this->volet_state);
		break;
	}

	battantDroit.loop();
	battantGauche.loop(); 
}




