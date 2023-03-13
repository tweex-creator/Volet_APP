#include "Volet.h"

Volet::Volet(uint8_t addr_i2c_droit, uint8_t addr_i2c_gauche):battantDroit(addr_i2c_droit), battantGauche(addr_i2c_gauche)
{
	
}

void Volet::config(configBattant bg, configBattant bd)
{
	battantDroit.config(bd);
	battantGauche.config(bg);
	battantDroit.setAutreBattant(&battantGauche);
	battantGauche.setAutreBattant(&battantDroit);
}

void Volet::calibrate()
{
	battantGauche.prepareCalibrate();
	battantDroit.prepareCalibrate();
	unsigned long time_prep_calibrate = millis();

	while (millis() - time_prep_calibrate < 5000) {
		battantGauche.LoopPrepareCalibrate();
		battantDroit.LoopPrepareCalibrate();
	}

	battantGauche.StopPrepareCalibrate();
	battantDroit.StopPrepareCalibrate();
	Serial.println("fin prep calibration");

	battantDroit.calibrate();
	battantGauche.calibrate();
	File calibration_file = LittleFS.open(F("/calibration_data.txt"), "w");
	unsigned long time_close_bd;
	unsigned long time_open_bd;
	unsigned long time_close_bg;
	unsigned long time_open_bg;

	time_close_bd = battantDroit.get_time_close();
	time_open_bd = battantDroit.get_time_close();
	time_close_bg = battantGauche.get_time_close();
	time_open_bg = battantGauche.get_time_close();
	if (calibration_file) {
		Serial.println("Write calibration data!");
		calibration_file.print(time_close_bd);
		calibration_file.print(time_open_bd);
		calibration_file.print(time_close_bg);
		calibration_file.print(time_open_bg);
	}
	else {
		Serial.println("Problem on create calibration file!");

	}

	calibration_file.close();

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

void Volet::setPosBG(float pos)
{
	battantGauche.setPosition(pos);
}

void Volet::setPosBD(float pos)
{
	battantDroit.setPosition(pos);
}

void Volet::loop()
{
	Serial.println("1");

	battantDroit.loop();
	Serial.println("2");

	battantGauche.loop();
	Serial.println("3");


}
