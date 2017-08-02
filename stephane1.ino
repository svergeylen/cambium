#include <aREST.h>
#include "DHT.h"
#define DHTPIN_A 2     // what digital pin we're connected to
#define DHTPIN_B 4     // what digital pin we're connected to

#define DHTTYPE DHT11   // DHT 21 (AM2301)
DHT dht_a(DHTPIN_A, DHTTYPE);
DHT dht_b(DHTPIN_B, DHTTYPE);

// Capteurs
const int pin_humidite = A0;  // Analog input pin that the potentiometer is attached to
float humidite = 0;           // Valeur de l'humidité du sol
int temperature;
int temperature_b;
int humidite_air;
int humidite_air_b;
int pinLuminosite=1; //autrement dit : A1
int luminosite;

// TEMPS
int toggle = 0;               // commute chaque seconde
int compteurSec = 0;          // compteur de secondes
int compteurCycles = 0;       // compteur de cycles total

// FSM
const int STATE_WAITING = 0;
const int STATE_MEAS = 1;
const int STATE_WATER = 2;
int state = STATE_WAITING;

//relais
int pinRelais = 3;
void luminositeCapteur(){
  luminosite = map(analogRead(pinLuminosite),8,800,0,100); //pourcentage entre les bornes
  Serial.println((String)"luminosite:"+ luminosite);
  //les 2 et 3 e valeur de map sont a changer en fonction de la luminosite de cet endroit
}
//captuer luminosite + temperature
void humiditeCapteur(){
  delay(1000);
    humidite_air = dht_a.readHumidity();    
    humidite_air_b = dht_b.readHumidity();    

  // Read temperature as Celsius (the default)
    temperature = dht_a.readTemperature();
    temperature_b = dht_b.readTemperature();
   Serial.println((String)"humidite_air:"+humidite_air);
  Serial.println((String)"temperature:"+temperature);
  Serial.println((String)"humidite_air_b:"+humidite_air_b);
  Serial.println((String)"temperature_b:"+temperature_b);

}

void loop() {
  Serial.println("debut");
  commute();
  mesure();
  humiditeCapteur();
  luminositeCapteur();

  switch (state) {

    // Attente
    case STATE_WAITING : {
      Serial.println((String)"#Cycle "+compteurCycles);
      Serial.println((String)"##Attente... "+compteurSec);
      if (compteurSec >= 20) {
        compteurSec = 0;
        compteurCycles++;
        state = STATE_MEAS;
      }
      break;
    }

    // Mesurage sur base des dernières données
    case STATE_MEAS : {
      
      Serial.println((String)"humidite_sol:"+humidite);

      if (humidite < 50) {
        state = STATE_WATER;
      }
      else {
        state = STATE_WAITING;
      }
      break;
    }
    
    // Arrosage
    case STATE_WATER : {
      Serial.println((String)"## Arrosage en cours..."+compteurSec);
      digitalWrite(pinRelais,LOW);
      if (compteurSec > 40) {
        compteurSec = 0;
        state = STATE_WAITING;
        digitalWrite(pinRelais,HIGH);

      }
      break;
    }
    default : {
      Serial.println("##STATE DEFAULT");
    }
  } // switch
    Serial.println("fin");
  
} // loop



void mesure() {
  float cur = map(analogRead(pin_humidite), 0, 1023, 100, 0);
  humidite = (humidite * 0.9) + (cur * 0.1);
  //Serial.print("cur = ");
  //Serial.print(cur);
  Serial.println((String)"humidite moyenne:"+humidite);
}


void commute() {
  delay(1000);
  compteurSec++;

  // Blink Alive
  if (toggle == 0) {
     toggle = 1;
     digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    toggle = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // humidité A0
  Serial.begin(9600);
  pinMode(pinRelais, OUTPUT);

  Serial.println("##=======   Arduino Uno Wifi - Cambium   ==========");
}

