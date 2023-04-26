#pragma once
#include <DHT_U.h>
#include <DHT.h>
#include <DFRobot_B_LUX_V30B.h>
#define typeDeDHT DHT22             // Ici, le type de DHT utilisé est un DHT22 (que vous pouvez changer en DHT11, DHT21, ou autre, le cas échéant)

class Capteurs
{
public:
	Capteurs(const int pinTempHumInt, const int pinTempHumExt, const int pinLuxExt, const int pinMouv);
	float getTempInt();
	float getHumInt();
	float getTempExt();
	float getHumExt();
	float getLuxExt();
	bool presenceDetected();

private:
	DHT temp_hum_int;
	DHT temp_hum_ext;
	DFRobot_B_LUX_V30B lux_ext;
	int pinMouv;
	unsigned long lastPresenceDetected;
	unsigned long coolDownPresence;
};

