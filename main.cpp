#include <Arduino.h>
#include <Servo.h>

const int pinTrig = 2; //pin digitale, car on veut stimuler la pin Trigger en la faisant passer a l'etat Haut
const int pinEcho = 3; //pin digitale, car la pin Echo est a l'etat Haut le temps que les ultrasons fassent l'aller-retour capteur/objet
const int pinBuzz = 9;

const int pinLedV = 8;
const int pinLedR = 6;
const int pinLedJ = 7;

byte * valeurs[30];
byte nbrVal = 30; //permet plus facilement de changer le nbr de valeurs que l'on veut memoriser
byte numVal = 0; //pour savoir a quelle case du tableau de valeurs nous sommes

byte distanceMax = 35; //distance a laquelle l'obstacle peut etre pour ouvrir la boite
bool obstacleDetect = false;

bool fermeturePrevue = true; //si une fermeture est envisagee (cad que le tableau de valeurs est remplid de 0 a >= 75%), True, sinon False
unsigned long tempsPourFermeture; //permet de lancer le compte a rebours avant fermeture

unsigned long temps; //temps entre le moment de l'emission des ultrasons et le moment de leur reception par le capteur
unsigned long tempsMax; //temps pour un objet situe au maximum de la distance atteignable par les ultrasons du capteur (400cm)
float distance;

Servo servo;

void setup() {

  Serial.begin(115200); Serial.println("Setup...");
  
  //Declaration
  servo.attach(5);
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT); 
  pinMode(pinLedV, OUTPUT);
  pinMode(pinLedR, OUTPUT);
  pinMode(pinLedJ, OUTPUT);
  pinMode(pinBuzz, OUTPUT);

  //Affichage de conseils d'utilisation
  //Serial.println(F("** CONSEILS D'UTILISATION:"));
  //Serial.println(F("1- Placer son obstacle à +/- 15° maximum de l'axe 0 (qui correspond à l'axe pile en face du récepteur (R) du capteur."));
  //Serial.println(F("2- Privilégier les obstacles plats et lisses."));
  //Serial.println(F("3 - Même si le capteur peut mesurer une distance entre 2 et 400cm, il a une meilleure précision pour des obstacles éloignés."));
  //Serial.println();
  //delay(3000);

  //Calcul de tempsMax
  float temp = 2.0 * 4.0 / 340.0; //utilisation d'une var temporaire car unsigned long ne peut contenir que des formes entieres
  tempsMax = temp * 1000000; //en micro sec
  Serial.print(F("Temps MAX = ")); Serial.print(tempsMax); Serial.println(F("us")); //us pour micro secondes

  //Reset preventif de la pin Trigger, car on ne connait pas le dernier etat de la pin avant lancement du code
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(20);

  //Initialisation du tableau de 50 valeurs
  for (int i = 0; i < nbrVal; i++) { valeurs[i] = 0; } 

  tempsPourFermeture = millis();
}

void loop() {
  
  //Envoi d'un signal Haut pendant 10 micro secondes a la pin Trigger
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);

  //Recuperation du temps de parcours de l'aller retour capteur/objet qui correspond au temps pendant lequel la pin Echo est a l'etat HIGH
  temps = pulseIn(pinEcho, HIGH, tempsMax); //en micro secondes ; si le temps calcule est > tempsMax, pulseIn renvoie 0
  Serial.print(F("Temps = ")); Serial.print(temps); Serial.println(F("us"));

  //Calcul de la distance capteur/objet a partir du temps obtenu
  distance = temps * 0.034/2; //en cm
  Serial.print(F("Distance = ")); Serial.print(distance); Serial.println(F("cm"));

  //Ajout de la nouvelle mesure dans le tableau de 50 valeurs
  if (distance < distanceMax && distance != 0) { valeurs[numVal] = 1; } else { valeurs[numVal] = 0; }
  if (numVal == nbrVal-1) { numVal = 0; } else { numVal++; } //si on est arrive au bout du tableau, on remplira au prochain tour la 1ere clase du tableau

  byte nbr0 = 0; byte nbr1 = 0; //pour comptabiliser le nombre de 0 et le nombre de 1 dans le tableau
  for (int i = 0; i < nbrVal; i++) {
    if (valeurs[i] == 1) { nbr1++; } else { nbr0++; }
  }

  if (nbr0 <= (nbrVal * 3/4)) { //si le nombre de 0 est inferieur a 75% du nombre de valeurs dans le tableau
    digitalWrite(pinLedV, HIGH);
    digitalWrite(pinLedR, LOW);
    digitalWrite(pinLedJ, LOW);
    
    servo.write(180);

    //Le bool permet de n'activer le buzzer qu'une seule fois, au moment ou l'obstacle est detecte
    if (obstacleDetect == false) {
      tone(pinBuzz, 800);
      delay(100);
      noTone(pinBuzz);
      obstacleDetect = true;
    }
    fermeturePrevue = false;

  } else {
    digitalWrite(pinLedV, LOW);
    digitalWrite(pinLedJ, HIGH);
    digitalWrite(pinLedR, LOW);

    if (fermeturePrevue == false) {
      tempsPourFermeture = millis(); //lancement du compte a rebours avant la potentielle fermeture (si pas d'obstacle detecte avant la fin du c.a.b)
      fermeturePrevue = true;
      obstacleDetect = false;
    }
  }

  if (fermeturePrevue == true && millis()-tempsPourFermeture>=5000) { //si compte a rebours lance && compte a rebours termine (on a atteint les 5s)
    digitalWrite(pinLedR, HIGH);
    digitalWrite(pinLedV, LOW);
    digitalWrite(pinLedJ, LOW);

    servo.write(0);
  }

  Serial.println();
  delayMicroseconds(10); //pause de 10ms minimum entre la fin de l'etat HIGH de la pin Echo et la prochaine stimulation de la pin Trigger

}
