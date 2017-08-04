#include <dht11.h>
dht11 DHT11;

// Entrées binaires
#define DHT11_serre 2
#define DHT11_ambiante 3

// Entrées analogiques
const int pin_humidite_sol = 0;
const int pin_luminosite_ambiante = 1;
const int pin_luminosite_ombre=2;
const int pin_pluie = 3;

// Sorties
int pinRelais = 4;

// Mesures
float humidite_sol = 0;
int temperature_serre = 0;
int humidite_serre = 0;
int temperature_ambiante = 0;
int humidite_ambiante = 0;
float luminosite_ambiante = 0;
float luminosite_ombre=0;
float pluie=0;
boolean pluie_en_cours = false;
// Temp
int toggle = 0;               // commute chaque seconde
int compteurSec = 0;          // compteur de secondes
int compteurCycles = 0;       // compteur de cycles total

// FSM
const int STATE_WAITING = 0;
const int STATE_MEAS = 1;
const int STATE_WATER = 2;
int state = STATE_WAITING;
boolean arrosage_en_cours = false;


void loop() {

  mesure();
  Serial.println("debut");
  Serial.println((String)"humidite_sol:"+humidite_sol);
  Serial.println((String)"luminosite_ambiante:"+luminosite_ambiante);
  Serial.println((String)"luminosite_ombre:"+luminosite_ombre);
  Serial.println((String)"pluie:"+pluie);
  Serial.println((String)"pluie_en_cours:"+pluie_en_cours);


  dht_serre();
  Serial.println((String)"humidite_serre:"+humidite_serre);
  Serial.println((String)"temperature_serre:"+temperature_serre);
  dht_ambiante();
  Serial.println((String)"humidite_ambiante:"+humidite_ambiante);
  Serial.println((String)"temperature_ambiante:"+temperature_ambiante);
  
  fsm();
  commute();
  Serial.println("fin");
}



void mesure() {
  float cur;
  int temp = analogRead(pin_humidite_sol);
  cur = map(temp, 0, 1023, 100, 0);
  humidite_sol = (humidite_sol * 0.9) + (cur * 0.1);
  // cur = map(analogRead(pin_luminosite_ambiante), 0, 1023, 0, 100);
  //luminosite_ambiante = 0; //(luminosite_ambiante * 0.9) + (cur * 0.1);
  luminosite_ambiante = map(analogRead(pin_luminosite_ambiante),0,1024,0,100);
  luminosite_ombre = map(analogRead(pin_luminosite_ombre),0,1024,0,100);
  pluie = map(analogRead(pin_pluie),0,1023,100,0);
  if(pluie > 60){
    pluie_en_cours= true;
  }else{
    pluie_en_cours = false;
  }

}

// Humidité et température SERRE
void dht_serre() {
  int chk = DHT11.read(DHT11_serre);
  if (chk == DHTLIB_OK) {
      temperature_serre = (float)DHT11.temperature;
      humidite_serre = (float)DHT11.humidity;
  }
  else { 
    Serial.println((String)"# DHT serre : erreur : "+chk);
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
    Serial.println((String)"# DHT ambiante : erreur : "+chk);
  }  
}

void fsm() {
  arrosage_en_cours = false;
  switch (state) {

    // Attente
    case STATE_WAITING : {
      Serial.println((String)"#Cycle "+compteurCycles);
      Serial.println((String)"#Attente... "+compteurSec);
      if (compteurSec >= 600) {
        compteurSec = 0;
        compteurCycles++;
        state = STATE_MEAS;
      }
      break;
    }

    // Decision d'arrosage ou non à ce moment précis
    case STATE_MEAS : {
      if (humidite_sol < 50 and humidite_sol > 8 and luminosite_ambiante < 50) {
        state = STATE_WATER;
      }
      else {
        state = STATE_WAITING;
      }
      break;
    }
    
    // Arrosage
    case STATE_WATER : {
      arrosage_en_cours = true;
      Serial.println((String)"#Arrosage en cours..."+compteurSec);
      digitalWrite(pinRelais,LOW);
      if (compteurSec > 600) {
        compteurSec = 0;
        state = STATE_WAITING;
        digitalWrite(pinRelais,HIGH);
      }
      break;
    }
    default : {
      Serial.println("# STATE DEFAULT");
    }
  } // switch
  Serial.println((String)"arrose:"+arrosage_en_cours);
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
  Serial.println("#=========   Arduino Uno Wifi - Cambium   ==========");
 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinRelais, OUTPUT);
  digitalWrite(pinRelais,HIGH);
}

