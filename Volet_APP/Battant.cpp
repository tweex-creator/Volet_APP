#include "Battant.h"

void Battant::calibrate() {
    Serial.println("debut calibration");

    is_calibrate = false;

    //fermeture du volet jusqu'a butté interrieur

    this->setDir(0);
    this->setSpeed(255);
    this->updatePontH();
    while (!this->isInStopperClose());
    this->setSpeed(0);
    this->updatePontH();


    // ouverture et mesure du temps 

    bool retry = false;

    do {
        if (retry) {
            Serial.println("Erreur lors de la calibration(temps incohérant), lancement de la recalibration de la phase d'ouverture");
            Serial.println("remise en pos initiale (fermé)");
            this->setDir(0);
            this->setSpeed(255);
            this->updatePontH();
            while (!this->isInStopperClose());
            this->setSpeed(0);
            this->updatePontH();
        }
    this->setDir(1);
    this->setSpeed(255);
    this->updatePontH();
    mesure_temps_ouverture = millis();
    while (!this->isInStopperOpen());
    this->setSpeed(0);
    this->updatePontH();
    this->tempsOuverture = millis() - mesure_temps_ouverture;//On enregistre le temps d'ouverture
    mesure_temps_ouverture = 0;
    Serial.print("Ouverture terminée, temps: ");
    Serial.println(this->tempsOuverture);


    retry = true;


    }while (this->tempsOuverture < MIN_TIME_OPEN_CLOSE_IN_MS);



    // fermeture et mesure du temps 
    retry = false;

    do {
        if (retry) {
            Serial.println("Erreur lors de la calibration(temps incohérant), lancement de la recalibration de la phase de fermeture");
            Serial.println("remise en pos initiale (ouvert)");
            this->setDir(1);
            this->setSpeed(255);
            this->updatePontH();
            while (!this->isInStopperOpen());
            this->setSpeed(0);
            this->updatePontH();
            Serial.println("OK");

        }
        this->setDir(0);
        this->setSpeed(255);
        this->updatePontH();
        mesure_temps_fermeture = millis();
        while (!this->isInStopperClose());
        this->setSpeed(0);
        this->updatePontH();
        this->tempsFermerture = millis() - mesure_temps_fermeture;//On enregistre le temps de fermeture 
        mesure_temps_fermeture = 0;
        retry = true;
        Serial.print("Fermeture terminée, temps: ");
        Serial.println(this->tempsFermerture);
    } while (this->tempsFermerture < MIN_TIME_OPEN_CLOSE_IN_MS);

    this->currentPos = 0;
    Serial.println("fin calibration");
}

void Battant::prepareCalibrate()
{
    Serial.println("debut prep pour calibration");
    this->setDir(1);
    this->setSpeed(255);
    this->updatePontH();
    Serial.println("ouverture legere");

   

}

void Battant::StopPrepareCalibrate()
{
    this->setDir(1);
    this->setSpeed(0);
    this->updatePontH();

}

void Battant::LoopPrepareCalibrate()
{
    if (this->isInStopperOpen()) {
        StopPrepareCalibrate();
    }
}

void Battant::set_time_close(long time_)
{
    this->tempsFermerture = time_;
}

void Battant::set_time_open(long time_)
{
    this->tempsOuverture = time_;

}

unsigned long Battant::get_time_close()
{
    return this->tempsFermerture;
}

unsigned long Battant::get_time_open()
{
    return this->tempsFermerture;
  
}

void Battant::setPosition(float pos)
{
    if(pos <= 100) targetPos = pos; // si la position est superieur a 100 on ne modifie la position voulue
          
}

void Battant::setMaxCouple(int max_)
{
    this->maxCouple = max_;
}

void Battant::loop() {

    this->updateSpeedAndDirForTarget();
    this->updatePontH();
    debug();
}

float Battant::getTargetPosition()
{
    return this->targetPos;
}

float Battant::getCurrentPosition(){
    float deplacement = 0;

    if (speed != 0) {

        if(this->lastMesurePosition == 0){
            lastMesurePosition = millis();
            return this->currentPos;
        }
        Serial.print("position ");
        Serial.print(battantType);
        Serial.print(" : ");

        unsigned long tempsParcours = millis() - lastMesurePosition;
        lastMesurePosition = millis();

        if (dir == 0) {
            deplacement = -100.0 * tempsParcours;
            deplacement = deplacement / tempsFermerture;
        }
        else {
            deplacement = 100.0 * tempsParcours;
            deplacement = deplacement / tempsOuverture;
        }
    }
    else {
        lastMesurePosition = 0;
    }
    currentPos = currentPos + deplacement;
 



    return this->currentPos;
}

void Battant::updateSpeedAndDirForTarget() {
    float trueTargetPos;
    if (battantType == 0) {//premier a ce fermer  securité antichevauchement
        if (this->getTargetPosition() < 10) {
            if (this->getAutreBatantPos() < 10) {
                trueTargetPos = 15;
            }
            else {
                trueTargetPos = getTargetPosition();
            }
        }
        else {
            trueTargetPos = getTargetPosition();
        }
    }
    else {//deuxième a ce fermer
        if (this->getTargetPosition() < 10) {
            if (this->getAutreBatantPos() > 0 && this->getAutreBatantPos() < 10) {
                trueTargetPos = 10;
            }
            else {
                trueTargetPos = getTargetPosition();
            }
        }
        else {
            trueTargetPos = getTargetPosition();
        }
    }

    if (trueTargetPos == 0) {
        if (getCurrentPosition() == 100) {
            mesure_temps_fermeture = millis();
        }
        if (!this->isInStopperClose()) {
            this->setDir(0);
            this->setSpeed(255);

        }
        else {
            if (mesure_temps_fermeture != 0) {
                this->tempsFermerture = millis() - this->mesure_temps_fermeture;
                this->mesure_temps_fermeture = 0;
            }
            setPosition(0);
            this->setSpeed(0);

        }
    }
    else if (trueTargetPos == 100 ) {
        if (getCurrentPosition() == 0) {
            this->mesure_temps_ouverture = millis();
        }
        if (!this->isInStopperOpen()) {
            this->setDir(1);
            this->setSpeed(255);
        }
        else {
            if (mesure_temps_ouverture != 0) {
                this->tempsOuverture = millis() - this->mesure_temps_ouverture;
                this->mesure_temps_ouverture = 0;
            }
            setPosition(100);
            this->setSpeed(0);

        }

    }else if (trueTargetPos > this->getCurrentPosition()+1) {
        this->setDir(1);
        this->setSpeed(255);
    }
    else if (trueTargetPos < this->getCurrentPosition()-1) {
        this->setDir(0);
        this->setSpeed(255);
    }
    else {
        this->setSpeed(0);
    }
}

void Battant::updatePontH()
{
    if (dir == 0) {
        inStopperOpen = false;
        analogWrite(pinFermeture, speed);
    }
    else {
        inStopperClose = false;
        analogWrite(pinOuverture, speed);
    }
}

void Battant::debug()
{
    if (this->battantType == 0) {
        auto debug_currentPos = this->currentPos;
        auto debug_TargetPos = this->targetPos;
    }
    else {
        auto debug_currentPos = this->currentPos;
        auto debug_TargetPos = this->targetPos;

    }
}

void Battant::setSpeed(int speed) {
    this->speed = speed;
}

void Battant::setDir(bool dir) {
    this->dir = dir;
}

bool Battant::isInStopperClose()
{
    if (-1*getCurrentCouple() > maxCouple) {
        if (firstTimeOverTorqueClose == 0) {
            firstTimeOverTorqueClose = millis();
           
        }
        if (millis() - this->firstTimeOverTorqueClose > this->maxTimeOverTorque) {
            inStopperClose = true;
            currentPos = 0;
        }
    }
    else {

        firstTimeOverTorqueClose = 0;
    }

    return inStopperClose;
}

bool Battant::isInStopperOpen()
{
    if (getCurrentCouple() > maxCouple) {
        if (firstTimeOverTorqueOpen == 0){
            firstTimeOverTorqueOpen = millis();
        }
        if (millis() - this->firstTimeOverTorqueOpen > this->maxTimeOverTorque) {
            inStopperOpen = true;
            currentPos = 100;

        }


    }
    else {
        firstTimeOverTorqueOpen = 0;
    }

    return inStopperOpen;
}

float Battant::getCurrentCouple()
{
    return cst_K * this->getIntensite();
}

int Battant::getAutreBatantPos()
{
    return autreBattant->getCurrentPosition();
}

void Battant::setAutreBattant(Battant* autreBattant_)
{
    this->autreBattant = autreBattant_;
}

float Battant::getIntensite()
{
    return ampVolet.getCurrent_mA();
}

Battant::Battant(uint8_t addr_i2c)
    : ampVolet(addr_i2c) {
    //while (ampVolet.begin(&Wire) != true) {
        Serial.print("ampVolet begin faild ");
        Serial.println(addr_i2c);
        delay(2000);
   // }
    ampVolet.setCalibration_32V_2A();
}

void Battant::config(configBattant battConf)
{
    this->battantType = battConf.battantType;
    this->pinOuverture = battConf.pont_H_pinOuverture;
    this->pinFermeture = battConf.pont_H_pinFermeture;
    this->pinFinDeCourseFerme = battConf.pinFinDeCourseFerme;
    pinMode(pinFinDeCourseFerme, INPUT);

    this->autreBattant = nullptr;
    speed = 0;
    dir = 0;

    this->firstTimeOverTorqueOpen = 0;
    this->maxTimeOverTorque = 100;
    this->inStopperOpen = 0;
    this->inStopperClose = 0;
    this->lastMesurePosition = 0;
    this->setMaxCouple(4000);
    max_time_bloque = 50;
    cst_K = 110.247;

    currentPos = 0;

}