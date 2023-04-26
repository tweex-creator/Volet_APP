#include "Battant.h"


//Calibration
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
bool Battant::calibration_inProgress()
{

    return this->calibration_var.state != 0;
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

void Battant::calibrate_loop()
{
    switch (this->calibration_var.state) {
    case -1:
        
        break;
    case 10:
        Serial.println("Debut calibration");
        this->calibration_var.mem_tempsFermerture = this->get_time_close();
        this->calibration_var.mem_tempsOuverture = this->get_time_open();
        this->setDir(1);//On ouvre suffisament le battant pour être sûre qu'il ne soit pas dans la zone de chevauchement (idem sur l'autre battant) 
        this->setSpeed(255);
        this->updatePontH();
        this->calibration_var.clock1 = millis();
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
        this->setDir(1);//On met le battant en postion ouverte 
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
        this->setDir(0);//On lance la fermeture et on declanche le chrono
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
            this->setSpeed(0);
            this->updatePontH();
        }
        break;
    case 34:
        this->calibration_var.state = 30;
        break;

    case 99:
        this->setDir(1);//On lance l'ouverture et on declanche le chrono se mettre dans une position intermédiaire qui laisse de la place pour calibrer l'autre battant
        this->setSpeed(255);
        this->updatePontH();
        this->calibration_var.clock1 = millis();
        this->calibration_var.state = 100;
        break;
    case 100:
        if (millis() - this->calibration_var.clock1 > 5000) {
            this->setSpeed(0);
            this->updatePontH();
            this->calibration_var.state = 0;
            this->battant_state = -1;
            if (battantType) {
                Serial.println("fin calibration battant gauche");
               
            }else{
                Serial.println("fin calibration battant droit");
            }
            Serial.print("Ouverture: ");
            Serial.print(this->tempsOuverture);
            Serial.print("  Fermeture: ");
            Serial.println(this->tempsFermerture);
        }
        break;
    default:
        this->calibration_var.state = -1;
        break;

    }
}



//Prise d'origine
bool Battant::init_priseOrigine() {
    if (this->setBattantState(-1)) {
        this->stop();
        this->origine_var.state = 10;
        return true;
    }
    return false;

}
bool Battant::priseOrigine_Ready()
{
    return this->origine_var.state == 12;
}
void Battant::priseOrigineNextStep() {
    if (this->priseOrigine_Ready()) {
        this->origine_var.state = 20;
    }
}
bool Battant::priseOrigine_inProgress()
{
    return (this->origine_var.state != 0);
}

void Battant::priseOrigine_loop()
{
    int local_origine_state = this->origine_var.state;
    switch (this->origine_var.state) {
    case 0:

        break;
    case 10:
        Serial.println("Debut prise Origine");

        this->setDir(1);//On ouvre suffisament le battant pour être sûre qu'il ne soit pas dans la zone de chevauchement (idem sur l'autre battant) 
        this->setSpeed(255);
        this->updatePontH();
        this->origine_var.clock1 = millis();
        this->origine_var.state = 11;
        break;
    case 11:
        //On ouvre le volet pendant 5 secondes ou si butée on s'arrete à la butée
        if (this->isInStopperOpen() || millis() - this->origine_var.clock1 > 5000) {
            this->setSpeed(0);
            this->updatePontH();
            this->origine_var.state = 12;
            Serial.println("attente confirmation");

        }
        break;
    case 12:

        break;
    case 20:
        Serial.println("debut mise en pos");

        //On ferme le battant
        this->setTargetPosition(0);
        this->setDir(0);
        this->setSpeed(255);
        this->updatePontH();
        this->origine_var.state = 21;
        break;
    case 21:
        //une fois le battant en butée, on considère la position actuelle comme 0
        if (this->isInStopperClose()) {
            Serial.println("butté atteinte");
            this->setDir(0);
            this->setSpeed(0);
            this->updatePontH();
            this->origine_var.state = 0;
            this->currentPos = 0;
            this->setBattantState(2);
        }
        break;
    default:
        break;

    }
}


//Gestion des obstacles
void Battant::initStopSecurity()
{
    if (battant_state != -4) {
        this->setSpeed(0);
        Serial.println("OBSTACLE!!");
        this->updatePontH();//On arrete tout de suite le volet pour gagner du temmps.
        emmergency_stopper_var.state = 0;
        this->setBattantState(-4);
    }
}

void Battant::handleStopSecurity() {
    switch (emmergency_stopper_var.state) {
        case 0:
            this->setDir(!dir);
            this->setSpeed(255);
            this->updatePontH();
            emmergency_stopper_var.clock1 = millis();
            emmergency_stopper_var.state = 1;
            Serial.println("secur 1");
            break;
        case 1:
            if (millis() - emmergency_stopper_var.clock1 > 2000) {
                this->setDir(!dir);
                this->setSpeed(0);
                this->updatePontH();
                emmergency_stopper_var.state = 2;
                Serial.println("secur 2");

            }
            break;
        case 2:

            //Attente d'une prise d'orgine
            break;

    }

}

//loop
void Battant::loop() {
    int battant_state_local = this->battant_state;
    getAndUpdateIntensite();
    switch (battant_state_local) {
        case -4:
            this->handleStopSecurity();
            break;
        case -3:

            break;
        case -2: {
            this->calibrate_loop();
            break;
        }
        case -1: {
            this->priseOrigine_loop();
            break;
        }
        case 0: {
            if (config_done == 1) {
                this->setBattantState(1);
            }
            break;
        }
        case 1: {
            this->setBattantState(-3);
            break;
        }
 
        default:
            this->updateSpeedAndDirForTarget();
            break;
    }

    if (overCurrentSecurityDetector() && battant_state_local != -4) {
        initStopSecurity();
    }

}


//configuration
Battant::Battant(uint8_t addr_i2c)
    : ampVolet(addr_i2c) {
    while (ampVolet.begin() != true) {
    Serial.print("ampVolet begin faild ");
    Serial.println(addr_i2c);
    delay(2000);
    }
    ampVolet.setCalibration_32V_2A();
    battant_state = 0;
    config_done = 0;
    for (int i = 0; i < N_MOYENNE_AMP; i++) {
        moyenneAmp[i] = 0;
    }
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
    this->maxTimeOverTorque = 0;
    this->inStopperOpen = 0;
    this->inStopperClose = 0;
    this->lastMesurePosition = 0;
    this->setMaxAmp(30.0);
    currentPos = 0;

    if (config_done < 2 && config_done != 1) {
        config_done = config_done + 2;
    }
    for (int i = 0; i < N_MOYENNE_AMP; i++) {
        moyenneAmp[i] = 0;
    }
    amp = 0;
}
void Battant::setAutreBattant(Battant* autreBattant_)
{
    this->autreBattant = autreBattant_;
    if (config_done > -1 && config_done != 1) {
        config_done = config_done - 1;

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

void Battant::setMaxAmp(float max_)
{
    this->maxAmp = max_;
}

//Contrôle Public
void Battant::setTargetPosition(float pos)
{
    if (pos <= 100 && pos >= 0) targetPos = pos; // si la position est superieur a 100 on ne modifie la position voulue

}
float Battant::getTargetPosition()
{
    return this->targetPos;
}
float Battant::getRealTargetPosition()
{
    return true_targetPos;
}

float Battant::getCurrentPosition() {
    float deplacement = 0;

    if (speed != 0) {

        if (this->lastMesurePosition == 0) {
            lastMesurePosition = millis();
            return this->currentPos;
        }
    
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

int Battant::getState()
{
    return this->battant_state;
}

//Contrôle Privé
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
bool Battant::setBattantState(int state_)
{
    if (battant_state == -4) {
        if (state_ != -1) {
            return false;
        }
    }
    battant_state = state_;
    Serial.print("newState: ");
    Serial.println(battant_state);

    return true;
}
void Battant::updatePontH()
{
    if (dir == 0) {
        inStopperOpen = false;
        analogWrite(pinFermeture, speed);
        analogWrite(pinOuverture, 0);

    }
    else {
        inStopperClose = false;
        analogWrite(pinOuverture, speed);
        analogWrite(pinFermeture, 0);

    }
}


//Detection de butées
bool Battant::isInStopperClose()
{
    if (-1 * getAndUpdateIntensite() > maxAmp) {
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
    if (getAndUpdateIntensite() > maxAmp) {
        if (firstTimeOverTorqueOpen == 0) {
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
bool Battant::overCurrentSecurityDetector()
{
    if (getAndUpdateIntensite() > maxAmp+20  || -1 * getAndUpdateIntensite() > maxAmp+20) {
        if (this->speed != 0) {
            return true;
        }

    }
    return false;
}
float Battant::getAndUpdateIntensite(bool instant)
{
    if (millis() - lastCurrentMesure > 100) {
        lastCurrentMesure = millis();
        for (int i = 0; i < N_MOYENNE_AMP-1; i++) {
            moyenneAmp[i] = moyenneAmp[i+1];
        }
        moyenneAmp[N_MOYENNE_AMP-1] = ampVolet.getCurrent_mA();
        for (int i = 0; i < N_MOYENNE_AMP; i++) {
            amp += moyenneAmp[i];
        }
        amp = amp / N_MOYENNE_AMP;
    
    }
 
    if (instant) {
        return  moyenneAmp[N_MOYENNE_AMP-1];
    }
    return amp;
}


void Battant::updateTrueTargetPos()
{

    if (this->battantType == 1) {
        if (this->getAutreBatantPos() < 15) {
            true_targetPos = 0;
        }
        else {
            true_targetPos = targetPos;
        }
    }
    else {
        if (this->targetPos <= 15 && this->getAutreBatantPos() != 0) {
            true_targetPos = 16;
        }
        else {
            true_targetPos = targetPos;
        }
    }
}

//Contrôle autonome de la position
void Battant::updateSpeedAndDirForTarget() {
    updateTrueTargetPos();
    float trueTargetPos = this->getRealTargetPosition();
    /*if (battantType == 0) {//premier a ce fermer  securité antichevauchement
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
    */
    if (trueTargetPos == 100) {
        if (!this->isInStopperOpen() || this->getCurrentPosition() < 100) {
            if (this->getCurrentPosition() > 100) {
                currentPos = 100;
            }
            this->setDir(1);
            this->setSpeed(255);
        }
        else{
            currentPos = 100;
            this->setDir(1);
            this->setSpeed(0); ;
        }
    }else if (trueTargetPos == 0) {
        if (!this->isInStopperClose() || this->getCurrentPosition() > 0) {
            if (this->getCurrentPosition() < 0) {
                currentPos = 0;
            }
            this->setDir(0);
            this->setSpeed(255);
        }
        else {
            currentPos = 0;
            this->setDir(0);
            this->setSpeed(0);
        }
    }  
    else if (trueTargetPos > this->getCurrentPosition()+1) {
       
           this->setDir(1);
           this->setSpeed(255);

    }
    else if (trueTargetPos < this->getCurrentPosition() - 1) {


        this->setDir(0);
        this->setSpeed(255);

    }
    else {
        this->setSpeed(0);
    }
   updatePontH();
}


int Battant::getAutreBatantPos()
{
    return autreBattant->getCurrentPosition();
}











