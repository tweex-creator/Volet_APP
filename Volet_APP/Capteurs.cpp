#include "Capteurs.h"

Capteurs::Capteurs(const int pinTempHumInt, const int pinTempHumExt, const int pinLumExt, const int pinMouv) :
	temp_hum_int(pinTempHumInt, typeDeDHT),
	temp_hum_ext(pinTempHumExt, typeDeDHT),
	pinMouv(pinMouv),
	lux_ext(pinLumExt, 13, 19)
{
	temp_hum_int.begin();
	temp_hum_ext.begin();
	lux_ext.begin();
	coolDownPresence = 5000;


}

float Capteurs::getTempInt()
{
	return temp_hum_int.readTemperature();
}

float Capteurs::getHumInt()
{
	return temp_hum_int.readHumidity();
}

float Capteurs::getTempExt()
{
	return temp_hum_ext.readTemperature();
}

float Capteurs::getHumExt()
{
	return temp_hum_ext.readHumidity();
}

float Capteurs::getLuxExt()
{
	return lux_ext.lightStrengthLux();
}

bool Capteurs::presenceDetected()
{
	if (digitalRead(pinMouv) == HIGH)
	{
		lastPresenceDetected = millis();
		return true;
	}
	else
	{
		if (millis() - lastPresenceDetected > coolDownPresence)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}
