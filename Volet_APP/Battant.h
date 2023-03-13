#pragma once
#include <Adafruit_INA219.h>
#define MIN_TIME_OPEN_CLOSE_IN_MS 5000
#define MAX_ANGLE_BOTH_BATTANT_WORK 10

class Battant;

struct configBattant {
	int pont_H_pinOuverture;
	int pont_H_pinFermeture;
	int pinFinDeCourseFerme;//pin ou se trouve le btn de fin de course
	bool battantType;//0 battant qui doit se fermer en premier, 1, battant qui doit se fermer en deuxième.


};

class Battant
{
public:
	void config(configBattant batConf);

	void calibrate();
	void prepareCalibrate();
	void StopPrepareCalibrate();
	void LoopPrepareCalibrate();
	void set_time_close(long time_);
	void set_time_open(long time_);
	unsigned long get_time_close();
	unsigned long get_time_open();

	Battant(uint8_t addr_i2c);
	
	void setPosition(float pos);  //definie la consigne de position du volet sur la valeur pos (en %)
	float getTargetPosition();
	float getCurrentPosition();
	
	float getCurrentCouple();
	int getAutreBatantPos();
	void setAutreBattant(Battant* autreBattant);

	//Configuration
	void setMaxCouple(int max);


	//utilisation
	void loop();

private:
	//Configuration initiale
	Adafruit_INA219 ampVolet;
	int pinOuverture;
	int pinFermeture;
	int max_time_bloque;
	int pinFinDeCourseFerme;

	int tempsOuverture;
	int tempsFermerture;
	bool battantType; //0 battant qui doit se fermer en premier, 1, battant qui doit se fermer en deuxième.
	float cst_K;               //110,247


	//configuration detection fin de course courant
	float maxCouple;
	unsigned int maxTimeOverTorque;
	unsigned long firstTimeOverTorqueOpen;
	unsigned long firstTimeOverTorqueClose;

	bool inStopperOpen;
	bool inStopperClose;

	unsigned long mesure_temps_ouverture;
	unsigned long mesure_temps_fermeture;

	//Memoire


	//Etat actuel
	bool is_calibrate;
	float currentPos;
	float targetPos;
	float trueTargetPos;
	float currentCouple;
	unsigned long lastMesurePosition;  //millis de la derniere mesure

	//Etat autre battant
	Battant* autreBattant;

	int speed;
	int dir;



	//Contrôle bas niveau
	void setSpeed(int speed);  //0 a 255
	void setDir(bool dir);     //0 fermeture, 1 ouverture
	bool isInStopperClose();
	bool isInStopperOpen();
	void updateSpeedAndDirForTarget();
	void updatePontH();

	void debug();
	


	float getIntensite();
	
	

	

};

