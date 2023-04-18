// capteursVolet.h

#ifndef _CAPTEURSVOLET_h
#define _CAPTEURSVOLET_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
//#include <DHT.h>
//#include <Adafruit_Sensor.h>
//#include <DFRobot_B_LUX_V30B.h>

class CapteursVolet {
public:
	CapteursVolet(unsigned long mouv_cool_downTime);
	float getLumExt();
	float getTempExt();

	float getTempInt();
	float getHumExt();
	float getHumInt();
	bool getMouvementInt();

private:
	//Luminosite
	//DFRobot_B_LUX_V30B lumExt;//The sensor chip is set to 13 pins, SCL and SDA adopt default configuration

	//temperature/humidite
		//Int
	//DHT dhtInt;
	long lastMesureIntTime;
	float lastMesureIntTempVal;
	float lastMesureIntHumVal;

	int pinTempIn;
		//Ext
	long lastMesureExtTime;
	float lastMesureExtTempVal;
	float lastMesureExtHumVal;

	//DHT dhtExt;
	int pinTempExt;

	//mouvement
	int pinMouv;
	unsigned long lastMouvTime;
	unsigned long coolDowntime;

};

#endif

