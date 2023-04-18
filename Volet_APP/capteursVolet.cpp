// 
// 
// 

#include "capteursVolet.h"

CapteursVolet::CapteursVolet(unsigned long mouv_cool_downTime)//:dhtInt(pinTempIn, DHT22), dhtExt(pinTempExt, DHT22)//,lumExt(27,22,21)
{
	this->lastMouvTime = 0;
	this->coolDowntime = mouv_cool_downTime;
	this->lastMesureIntTime = 0;
	this->lastMesureIntHumVal = 0;
	this->lastMesureIntTempVal = 0;
	this->lastMesureExtTime = 0;
	this->lastMesureExtHumVal = 0;
	this->lastMesureExtTempVal = 0;

	pinTempIn = 35;
	pinTempExt = 26;
	pinMouv = 14;
	//pinMode(pinMouv, INPUT);
	

	//dhtInt.begin();
	//dhtExt.begin();
//	lumExt.begin();

}

float CapteursVolet::getLumExt()
{
	return 0.0;// lumExt.lightStrengthLux();
}

float CapteursVolet::getTempExt()
{
	if (millis() - lastMesureExtTime < 3000) return lastMesureExtTempVal;
	lastMesureExtTime = millis();
	//lastMesureExtTempVal = dhtExt.readTemperature();
	//lastMesureExtHumVal = dhtExt.readHumidity();
	return 0;// lastMesureExtTempVal;
}

float CapteursVolet::getTempInt()
{
	if (millis() - lastMesureIntTime < 3000) return lastMesureIntTempVal;
	lastMesureIntTime = millis();
	//lastMesureIntTempVal = dhtInt.readTemperature();
	//lastMesureIntHumVal = dhtInt.readHumidity();
	return 0;// lastMesureIntTempVal;
}

float CapteursVolet::getHumExt()
{
	if (millis() - lastMesureExtTime < 3000) return lastMesureExtHumVal;
	lastMesureExtTime = millis();
	//lastMesureExtTempVal = dhtExt.readTemperature();
	//lastMesureExtHumVal = dhtExt.readHumidity();
	return 0;// lastMesureExtHumVal;
}

float CapteursVolet::getHumInt()
{
	if (millis() - lastMesureIntTime < 3000) return lastMesureIntHumVal;
	lastMesureIntTime = millis();
	//lastMesureIntTempVal = dhtInt.readTemperature();
	//lastMesureIntHumVal = dhtInt.readHumidity();
	return 0;// lastMesureIntHumVal;
}

bool CapteursVolet::getMouvementInt()
{
	bool mouv;
	if (digitalRead(pinMouv)) lastMouvTime = millis();

	if (millis() - lastMouvTime > coolDowntime ) {
		mouv = true;
	}
	else {
		mouv = false;
	}
	return mouv;
}
