#include "Capteurs.h"

Capteurs::Capteurs(const int pinTempHumInt, const int pinTempHumExt, const int pinLumExt):
	temp_hum_int(pinTempHumInt, typeDeDHT),
	temp_hum_ext(pinTempHumExt, typeDeDHT),
	lux_ext(pinLumExt)
{
	temp_hum_int.begin();
	temp_hum_ext.begin();
	//lux_ext.begin();


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
