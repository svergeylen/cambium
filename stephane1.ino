
// Capteurs
const int pin_humidite = A0;  // Analog input pin that the potentiometer is attached to
float humidite = 0;           // Valeur de l'humidité du sol

// TEMPS
int toggle = 0;               // commute chaque seconde
int compteurSec = 0;          // compteur de secondes
int compteurCycles = 0;       // compteur de cycles total

// FSM
const int STATE_WAITING = 0;
const int STATE_MEAS = 1;
const int STATE_WATER = 2;
int state = STATE_WAITING;




void loop() {
  commute();
  mesure();

  switch (state) {

    // Attente
    case STATE_WAITING : {
      Serial.print("Cycle ");
      Serial.print(compteurCycles);
      Serial.print(" Attente... ");
      Serial.println(compteurSec);
      if (compteurSec >= 5) {
        compteurSec = 0;
        compteurCycles++;
        state = STATE_MEAS;
      }
      break;
    }

    // Mesurage sur base des dernières données
    case STATE_MEAS : {
      
      Serial.print("\t humidite moyenne = ");
      Serial.println(humidite);

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
      Serial.print("Arrosage en cours...");
      Serial.println(compteurSec);
      if (compteurSec > 20) {
        compteurSec = 0;
        state = STATE_WAITING;
      }
      break;
    }
    default : {
      Serial.println("STATE DEFAULT");
    }
  } // switch
    
} // loop



void mesure() {
  float cur = map(analogRead(pin_humidite), 0, 1023, 100, 0);
  humidite = (humidite * 0.9) + (cur * 0.1);
  //Serial.print("cur = ");
  //Serial.print(cur);
  //Serial.print("\t humidite moyenne = ");
  //Serial.println(humidite);
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
  Serial.println("=======   Arduino Uno Wifi - Cambium   ==========");
}

