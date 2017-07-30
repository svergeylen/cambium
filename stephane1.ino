#include <dht11.h>
dht11 DHT11;

// Entrées binaires
#define DHT11_serre 2
#define DHT11_ambiante 3

// Entrées analogiques
const int pin_humidite_sol = A0;
const int pin_luminosite_ambiante = A1;

// Sorties
int pinRelais = 3;

// Mesures
float humidite_sol = 0;
int temperature_serre = 0;
int humidite_serre = 0;
int temperature_ambiante = 0;
int humidite_ambiante = 0;
float luminosite_ambiante = 0;

// Temp
int toggle = 0;               // commute chaque seconde
int compteurSec = 0;          // compteur de secondes
int compteurCycles = 0;       // compteur de cycles total

// FSM
const int STATE_WAITING = 0;
const int STATE_MEAS = 1;
const int STATE_WATER = 2;
int state = STATE_WAITING;



void loop() {
  mesure();
  Serial.print("Humidite sol = ");
  Serial.println(humidite_sol);
  Serial.print("Luminosité = ");
  Serial.println(luminosite_ambiante);
  
  dht_serre();
  Serial.println(humidite_serre);
  Serial.println(temperature_serre);
  dht_ambiante();
  Serial.println(humidite_ambiante);
  Serial.println(temperature_ambiante);
  
  fsm();
  commute();
}



void mesure() {
  float cur;
  cur = map(analogRead(pin_humidite_sol), 0, 1023, 100, 0);
  humidite_sol = (humidite_sol * 0.9) + (cur * 0.1);
  cur = map(analogRead(pin_luminosite_ambiante), 0, 1023, 0, 100);
  luminosite_ambiante = (luminosite_ambiante * 0.9) + (cur * 0.1);
}

// Humidité et température SERRE
void dht_serre() {
  int chk = DHT11.read(DHT11_serre);
  if (chk == DHTLIB_OK) {
      temperature_serre = (float)DHT11.temperature;
      humidite_serre = (float)DHT11.humidity;
  }
  else { 
    Serial.print("DHT serre : erreur : "); 
    Serial.println(chk);
  }  
}


// Humidité et température AMBIANTE
void dht_ambiante() {
  int chk = DHT11.read(DHT11_ambiante);
  if (chk == DHTLIB_OK) {
      temperature_ambiante = (float)DHT11.temperature;
      humidite_ambiante = (float)DHT11.humidity;
  }
  else { 
    Serial.print("DHT ambiante : erreur : "); 
    Serial.println(chk);
  }  
}

void fsm() {
  switch (state) {

    // Attente
    case STATE_WAITING : {
      Serial.print("Cycle ");
      Serial.print(compteurCycles);
      Serial.print(" Attente... ");
      Serial.println(compteurSec);
      if (compteurSec >= 3600) {
        compteurSec = 0;
        compteurCycles++;
        state = STATE_MEAS;
      }
      break;
    }

    // Decision d'arrosage ou non à ce moment précis
    case STATE_MEAS : {
      if (humidite_sol < 50) {
        state = STATE_WATER;
      }
      else {
        state = STATE_WAITING;
      }
      break;
    }
    
    // Arrosage
    case STATE_WATER : {
      Serial.print("Arrosage en cours...");
      Serial.println(compteurSec);
      digitalWrite(pinRelais,LOW);
      if (compteurSec > 200) {
        compteurSec = 0;
        state = STATE_WAITING;
        digitalWrite(pinRelais,HIGH);
      }
      break;
    }
    default : {
      Serial.println("STATE DEFAULT");
    }
  } // switch
}


// Blink Alive et delay 1 seconde
void commute() {
  compteurSec++;
  if (toggle == 0) {
     toggle = 1;
     digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    toggle = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(1000);
}


void setup() {
  Serial.begin(9600);
  Serial.println("=========   Arduino Uno Wifi - Cambium   ==========");
 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinRelais, OUTPUT);
}

