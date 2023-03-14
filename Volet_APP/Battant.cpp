#include "Battant.h"

bool Battant::init_calibration() {
    if (this->setBattantState(-2)) {
        this->stop();
        this->calibration_var.state = 10;
        return true;
    }
    return false;

}
bool Battant::calibration_Ready()
{
    return this->calibration_var.state == 12;
}
void Battant::calibrateNextStep() {
    if (this->calibration_Ready()) {
        this->calibration_var.state = 20;
    }
}
void Battant::calibrate_loop()
{
    switch (this->calibration_var.state) {
    case -1:
        
        break;
    case 10:
        Serial.println("Debut calibration");
        this->calibration_var.clock1 = millis();
        this->calibration_var.mem_tempsFermerture = this->get_time_close();
        this->calibration_var.mem_tempsOuverture = this->get_time_open();
        this->setDir(1);//On ouvre suffisament le battant pour être sûre qu'il ne soit pas dans la zone de chevauchement (idem sur l'autre battant) 
        this->setSpeed(255);
        this->updatePontH();
        this->calibration_var.state = 11;

        break;
    case 11:
        //On ouvre le volet pendant 5 secondes ou si butée on s'arrete à la butée
        if (this->isInStopperOpen() || millis() - this->calibration_var.clock1 > 5000) {
            this->setSpeed(0);
            this->updatePontH();
            this->calibration_var.state = 12;
        }
        break;
    case 12:

        break;
    case 20:
        this->setDir(0);//On met le battant en postion fermé 
        this->setSpeed(255);
        this->updatePontH();
        this->calibration_var.clock1 = millis();
        this->calibration_var.state = 21;

        break;
    case 21:
        if (millis() - this->calibration_var.clock1 > 30000) {// si le volet met un temps anormale a atteindre la position fermé, on le met en erreur
            this->calibration_var.state = -1; 
        }
        if (this->isInStopperClose()) {//Une fois le battant en butté on l'arrete puis on passe à l'étape suivante.
            this->setSpeed(0);
            this->updatePontH();
            this->calibration_var.state = 22;
        }
        break;

    case 22:
        this->setDir(1);//On lance l'ouverture et on declanche le chrono
        this->setSpeed(255);
        this->updatePontH();
        this->calibration_var.clock1 = millis();
        this->calibration_var.state = 23;

        break;

    case 23:
       
        if (this->isInStopperOpen()) {
            this->setSpeed(0);
            this->updatePontH();
            this->set_time_open(millis() - this->calibration_var.clock1);//On enregistre le temps d'ouverture
            if (get_time_open() < MIN_TIME_OPEN_CLOSE_IN_MS) {
                this->calibration_var.state = 24;//Si le temps est annormalement rapide, on recommence
            }
            else {
                this->calibration_var.state = 30;//Sinon, on passe a la suite
            }

        }else if (millis() - this->calibration_var.clock1 > 30000) {// si le battant met un temps anormalement long à s'ouvrire , on le met en erreur
            this->calibration_var.state = -1;
        }
        break;
    case 24:
        this->calibration_var.state = 20;       
        break;

    case 30:
        this->setDir(1);//On met le battant en postion fermé 
        this->setSpeed(255);
        this->updatePontH();
        this->calibration_var.clock1 = millis();
        this->calibration_var.state = 31;

        break;
    case 31:
        if (millis() - this->calibration_var.clock1 > 30000) {// si le volet met un temps anormale a atteindre la position fermé, on le met en erreur
            this->calibration_var.state = -1;
        }
        if (this->isInStopperOpen()) {//Une fois le battant en butté on l'arrete puis on passe à l'étape suivante.
            this->setSpeed(0);
            this->updatePontH();
            this->calibration_var.state = 32;
        }
        break;

    case 32:
        this->setDir(0);//On lance l'ouverture et on declanche le chrono
        this->setSpeed(255);
        this->updatePontH();
        this->calibration_var.clock1 = millis();
        this->calibration_var.state = 33;

        break;

    case 33:

        if (this->isInStopperClose()) {
            this->setSpeed(0);
            this->updatePontH();
            this->set_time_close(millis() - this->calibration_var.clock1);//On enregistre le temps d'ouverture
            if (get_time_close() < MIN_TIME_OPEN_CLOSE_IN_MS) {
                this->calibration_var.state = 34;//Si le temps est annormalement rapide, on recommence
            }
            else {
                this->calibration_var.state = 99;//Sinon, on passe a la suite
            }

        }
        else if (millis() - this->calibration_var.clock1 > 30000) {// si le battant met un temps anormalement long à s'ouvrire , on le met en erreur
            this->calibration_var.state = -1;
        }
        break;
    case 34:
        this->calibration_var.state = 30;
        break;

    case 99:
        this->setDir(1);//On lance l'ouverture et on declanche le chrono
        this->setSpeed(255);
        this->updatePontH();
        this->calibration_var.clock1 = millis();
        this->calibration_var.state = 30;
        break;
    case 100:
        if (millis() - this->calibration_var.clock1 > 5000) {
            this->setSpeed(0);
            this->updatePontH();
            this->calibration_var.state = 0;
            Serial.println("fin calibration");
        }
        break;
    default:
        this->calibration_var.state = -1;
        break;

    }
}

void Battant::setTarget(float pos)
{
    if(pos <= 100 && pos >= 100) targetPos = pos; // si la position est superieur a 100 on ne modifie la position voulue
          
}

void Battant::setMaxCouple(int max_)
{
    this->maxCouple = max_;
}

void Battant::loop() {
    switch (battant_state) {
        case -3:

            break;
        case -2:
            this->calibrate_loop();
            break;
        case -1:

            break;
        case 0:

            break;
        case 1:

            break;
        default:
            break;

    }

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

   if (trueTargetPos > this->getCurrentPosition()+1) {
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

void Battant::cancel_calibration()
{
    if (this->calibration_var.state != 0) { // si le battant est en cours de calibration on annule sinon rien
        this->stop();
        this->calibration_var.state = 0;
        this->set_time_open(this->calibration_var.mem_tempsOuverture);
        this->set_time_close(this->calibration_var.mem_tempsFermerture);
        this->setBattantState(-3);
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

bool Battant::setBattantState(char state)
{
    this->battant_state = state;
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

void Battant::stop()
{
    this->setSpeed(0);
    this->updatePontH();
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


    this->autreBattant = nullptr;
    speed = 0;
    dir = 0;

    this->firstTimeOverTorqueOpen = 0;
    this->maxTimeOverTorque = 100;
    this->inStopperOpen = 0;
    this->inStopperClose = 0;
    this->lastMesurePosition = 0;
    this->setMaxCouple(4000);
    cst_K = 110.247;

    currentPos = 0;

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