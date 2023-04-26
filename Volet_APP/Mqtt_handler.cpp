#include "Mqtt_handler.h"
#include "Capteurs.h"
#include "Volet.h"
Mqtt_handler* Mqtt_ptr_callback = nullptr;

void callback_ext(char* topic, byte* message, unsigned int length) {
    if (Mqtt_ptr_callback == nullptr) {
        return;
    }
    Mqtt_ptr_callback->callback(topic, message, length);
}



Mqtt_handler::Mqtt_handler(const char* mqtt_server, Volet* volet, Capteurs* capteurs):client(espClient), volet(volet), capteurs(capteurs)
{
	this->mqtt_server = mqtt_server;
	Mqtt_ptr_callback = this;
	client.setServer(mqtt_server, 1883);
    client.setCallback(callback_ext);
    lastReconnectAttempt = -1; // -1 allows the first attempt to be made immediately
    id = "volet_salon";
}

Mqtt_handler::~Mqtt_handler()
{
    this->mqtt_server = nullptr;
}

void Mqtt_handler::loop()
{
    if (!client.connected()) {
		this->reconnect();
    }
    if (client.connected()) {
        if (millis() - lastSendRecap > 500) {
            lastSendRecap = millis();
            this->sendRecap();
        }
        client.loop();
    }
}

void Mqtt_handler::reconnect()
{
    if (this->lastReconnectAttempt > 0 && millis() - this->lastReconnectAttempt < 5000) {
		return;
	}
    this->lastReconnectAttempt = millis();
    // Loop until we're reconnected
    if (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(id, "user_mqtt", "mqtt12345")) {
            Serial.println("connected");
            // Subscribe
            this->subscribe();
        }
        else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
        }
    }
}

void Mqtt_handler::subscribe()
{
    String id(this->id);
    String topic = "home/" + id + "/position/battant_gauche/set";
    client.subscribe(topic.c_str());

    topic = "home/" + id + "/position/battant_droit/set";
    client.subscribe(topic.c_str());

    topic = "home/" + id + "/commande/calibrate";
    client.subscribe(topic.c_str());

    topic = "home/" + id + "/commande/origine";
    client.subscribe(topic.c_str());

    topic = "home/" + id + "/position/volet/set";
    client.subscribe(topic.c_str());
}

void Mqtt_handler::callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    String id(this->id);
    String topic_check = "home/" + id + "/position/battant_gauche/set";

    if (String(topic) == topic_check) {
        //get the float value
        int pos = messageTemp.toInt();
        volet->setPosBG(pos);
    }

    topic_check = "home/" + id + "/position/volet/set";

    if (String(topic) == topic_check) {
        //get the float value
        int pos = messageTemp.toInt();
        volet->setPosBG(pos);
        volet->setPosBD(pos);

    }

    topic_check = "home/" + id + "/position/battant_droit/set";
    if (String(topic) == topic_check) {
		//get the float value
		int pos = messageTemp.toInt();
		volet->setPosBD(pos);
	}

    topic_check = "home/" + id + "/commande/calibrate";
    if (String(topic) == topic_check) {
		volet->calibrate();
	}

    topic_check = "home/" + id + "/commande/origine";
    if (String(topic) == topic_check) {
		volet->priseOrigine();
	}


}

void Mqtt_handler::sendRecap() {

    // Send recap
    String id(this->id);
    String topic = "home/" + id + "/position/battant_gauche";
	client.publish(topic.c_str(), String(volet->getPosBG()).c_str());

    topic = "home/" + id + "/position/battant_droit";
    client.publish(topic.c_str(), String(volet->getPosBD()).c_str());

    topic = "home/" + id + "/capteurs/temp/int";
    client.publish(topic.c_str(), String(capteurs->getTempInt()).c_str());

    topic = "home/" + id + "/capteurs/hum/int";
    client.publish(topic.c_str(), String(capteurs->getHumInt()).c_str());

    topic = "home/" + id + "/capteurs/temp/ext";
    client.publish(topic.c_str(), String(capteurs->getTempExt()).c_str());

    topic = "home/" + id + "/capteurs/hum/ext";
    client.publish(topic.c_str(), String(capteurs->getHumExt()).c_str());

    topic = "home/" + id + "/capteurs/lum/ext";
    client.publish(topic.c_str(), String(capteurs->getLuxExt()).c_str());

    topic = "home/" + id + "/capteurs/mouv/int";
    client.publish(topic.c_str(), String(capteurs->presenceDetected()).c_str());
}
