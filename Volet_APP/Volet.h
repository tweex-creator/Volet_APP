#pragma once
#include "Battant.h"

class Volet
{
public:
	//Configuration
	Volet(uint8_t addr_i2c_droit, uint8_t addr_i2c_gauche);
	void config(configBattant bg, configBattant bd);
	
	//Calibration
	void calibrate();
	bool load_calibration();
	void calibrate_manual(long time_close_bd, long time_open_bd, long time_close_bg, long time_open_bg);

	//Prise origine
	void priseOrigine();

	//Contôle public
	void setPosBG(float pos);
	void setPosBD(float pos);

	//Récupération des infos
	float getPosBG();
	float getPosBD();
	float getTargetBD();
	float getTargetBG();
	float getRealTargetBD();
	float getRealTargetBG();

	//loop
	void loop();

private:
	Battant battantDroit;
	Battant battantGauche;

	///Variables d'états general
	/*
	indique l'état actuel du battant:
	   -4 : temporairement indisponnible (un des battant n'est pas en mesure de recevoire une commande)
	   -3 : En erreur
	   -2 : en calibration
	   -1 : prise d'origine
		0 : En attente de configuration
		1 : Demarrage
		2 : Pret
	*/
    int volet_state;
	bool config_done;



	///Variable de calibrate_loop (pour fonctionnement assync)
	struct Calibration_var_set {
		int state;
		/* calibration_state
		indique l'avancement actuel de la calibration:
		0  : pas de calibration en cours
		10 : lancement calibration
		11 : En preparation pour calibration
		20 : Lancement calibration battant droit
		21 : Calibration battant droit
		30 : Lancement calibration battant gauche
		31 : Calibration battant gauche
		*/
	};

	Calibration_var_set calibration_var;
	struct Origine_var_set {
		int state;
		/* origine_var.state
		indique l'avancement actuel de la prise d'origine:
		0  : pas de prise d'origine en cours
		10 : lancement de la prise d'origine
		11 : En preparation pour  prise d'origine
		20 : Lancement  prise d'origine battant droit
		21 : Prise d'origine battant droit
		30 : Lancement prise d'origine battant gauche
		31 : Prise d'origine battant gauche
		*/
		unsigned long clock;
	};
	Origine_var_set origine_var;


	//config 
	bool setVoletState(int state);

	/// loops
	void calibration_loop();
	void priseOrigine_loop();


};

