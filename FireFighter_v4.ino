#define EnA 6
#define EnB 5
#define In1 13
#define In2 12
#define In3 11
#define In4 10
#define echo 3
#define trigger 4

#include <Servo.h>

#define PIN_MOTOR 9
#define PIN_POTI  A2



Servo servo;
unsigned long startTime, delayTime = 1000; // in milliseconds
byte fan_speed = 50; //Drehgeschwindigkeit des Zentrifugalkompressors
byte stop = 30; //PWM-Signal zum Stoppen des Zentrifugalkompressors

//Geschwindigkeitsvariablen für die PWM zum Fahren des Fahrzeugs
int speed = 180;   // Drehgeschwindigkeit; max. 255
int vspeed = 100;  // Vorwärtsgeschwindigkeit; max. 255

int pins[5] = { A1, A2, A3, A4, A5 };  // Pins des Flammensensors zum Auslesen
int fwert[5] = { 0, 0, 0, 0, 0 };      // Analogwerte des Flammensensors der einzelnen Pins
int fdirect[5] = { 0, 0, 0, 0, 0 };    // Array gibt die Richtung an wo die Kerze steht; nur eine 1 möglich; das erste Element ist ganz rechts


long entfernung = 0; // Variable zum Speichern der Entfernung von Fahrzeug zu etwas vor dem Fahrzeug
long zeit = 0; // Variable zum Speichern der Zeit des Ultraschallsensors


void setup() //Festlegen des Setups
{
  // Starte Serielle Ausgabe
  Serial.begin(9600);

  // deklariere Analoge-Pins 1-5 als INPUT
  for (int i = 0; i < 5; i++)
  {
    pinMode(pins[i], INPUT);
  }

  // Pins als Output/Input deklarieren
  
  //Motorsteuergerät
  pinMode(EnA, OUTPUT);
  pinMode(EnB, OUTPUT);
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(In3, OUTPUT);
  pinMode(In4, OUTPUT);

  // Ultraschallsensor
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);

  //ESC (Drohnenmotor) aktivieren
  startTime = micros();
  servo.attach(PIN_MOTOR);
  servo.write(25); // needed for initializing the ESC

  delay(3000);
}





void loop() //Normaler Programmablauf; wird immer wiederholt
{
  flammensensor();  // Auslesen und Auswerten des Flammensensors; Werte werden im oben genannten Arrays gespeichert und überschrieben
  ausrichtung();    // Nachdem der Flammensensor ausgelesen wurde wird er nun ausgewertet und eine Dreh- oder Fahrbewegung wird ausgeführt
}






void flammensensor() //Auslesen und Auswerten des Flammensensors; Werte werden im oben genannten Arrays gespeichert und überschrieben
{
  int fdirectIndex = 0;  // Index des maximalen Werts
  int fdirectValue = 0;  // maximaler Wert

  for (int i = 0; i < 5; i++) //Auslesen des Flammensensors mit einer For-Schleife
  {
    fwert[i] = analogRead(pins[i]); // Auslesen der Analogwerte des Flammensensors und eintragung der Werte im Array "fwert"
    Serial.print("A");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(fwert[i]);

    // Überprüfen, ob der aktuelle Wert größer als der bisherige Maximalwert ist
    // Wenn der Wert "fwert" größer ist als "fdirectValue", wird der neue Wert in "fdirectValue" geschrieben. 
    // Anschließend wird die Nummer des Elements in "fdirectIndex" geschrieben.
    if (fwert[i] > fdirectValue) {
      fdirectValue = fwert[i];
      fdirectIndex = i;
    }


    // Ausgabe des fdirect-Arrays
    Serial.print("fdirect: ");
    for (int i = 0; i < 5; i++) {
      Serial.print(fdirect[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  // Der Ausdruck fdirect[i] = (i == fdirectIndex) ? 1 : 0; setzt das i-te Element im Array fdirect auf 1, wenn i gleich fdirectIndex ist, andernfalls wird es auf 0 gesetzt.
  for (int i = 0; i < 5; i++) {
    fdirect[i] = (i == fdirectIndex) ? 1 : 0;
  }
}





void ausrichtung() // Nachdem der Flammensensor ausgelesen wurde wird er nun ausgewertet und eine Dreh- oder Fahrbewegung wird ausgeführt
{
  // ausrichtung mithilfe von fdirect ausführen
  for (int i = 0; i < 5; i++) // erhöht die Variable "i", um mit der nachfolgenden if-Bedinung das Array für die Richtung durchzugehen.
  {
    if (fdirect[i] == 1) // Wenn das Element im Array "1" ist, wird mit der Variable "i" der entsprechnede Case ausgewählt und das Fahrzeug fährt
    {
      switch (i) {
        case 0:
          rechts();
          Serial.println("Rechts fahren");
          break;
        case 1:
          rechts();
          Serial.println("Leicht Rechts fahren");
          break;
        case 2:
          vorwaerts();
          Serial.println("Vorwärts fahren");
          break;
        case 3:
          links();
          Serial.println("Leicht Links fahren");
          break;
        case 4:
          links();
          Serial.println("Links fahren");
          break;
      }
    }
  }
}


void ultraschallsensor() // Werte aus dem Ultraschallsensor auslesen und in "entfernung" speichern
{
  while (entfernung <= 10)
  {
  // Sender kurz ausschalten um Störungen des Signal zu vermeiden
  digitalWrite(trigger, LOW);
  delay(5);

  // Signal für 10 Microsekunden senden, danach wieder ausschalten
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  // pulseIn -> Zeit messen, bis das Signal zurückkommt
  long zeit = pulseIn(echo, HIGH);

  // Entfernung in cm berechnen
  // Zeit/2 -> nur eine Strecke
  entfernung = (zeit / 2) * 0.03432;

  // Ausgabe Entfernung in cm
  Serial.print(entfernung);
  Serial.println("cm");
  }
  // alle Motoren ausschalten
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
  loeschung();
}


void loeschung() // aktivieren des Zentrifugalkompressors zum Löschen der Kerze
{
  // starte Löschung
  if ((millis() - startTime) >= delayTime && Serial.available() == 1) {

    fan_speed = 50; // Motorgeschwindigkeit von 30 bis 110,  30 nix -- 110 alles was geht
    servo.write(fan_speed);
    Serial.println("Fan an");
    startTime = micros();
    delay(4000);
    servo.write(stop);
  }

  delay(10000);
}


void vorwaerts()  //beide Motoren vorwärts drehen lassen
{
  // Motor A einschalten
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  // Geschwindigkeit 150 von max. 255
  analogWrite(EnA, speed);
  // Motor B einschalten
  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
  // Geschwindigkeit 150 von max. 255
  analogWrite(EnB, vspeed);
  // Starte Ultraschalldistanz Messung
  ultraschallsensor();
}


void rueckwaerts()  //beide Motoren rückwärts drehen lassen
{
  // Motor A einschalten
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  // Geschwindigkeit 150 von max. 255
  analogWrite(EnA, speed);
  // Motor B einschalten
  digitalWrite(In3, LOW);
  digitalWrite(In4, HIGH);
  // Geschwindigkeit 150 von max. 255
  analogWrite(EnB, speed);
  delay(3000);
  // alle Motoren ausschalten
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
}


void links()  //Motor A dreht vorwärts, Motor B dreht rückwärts; "fstop" stoppt die Drehung
{
  // Motor A einschalten
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  // Geschwindigkeit 150 von max. 255
  analogWrite(EnA, speed);
  // Motor B einschalten
  digitalWrite(In3, LOW);
  digitalWrite(In4, HIGH);
  // Geschwindigkeit 150 von max. 255
  analogWrite(EnB, speed);
  fstop();
}


void rechts()  //Motor A dreht rückwärts, Motor B dreht vorwärts; "fstop" stoppt die Drehung
{
  // Motor A einschalten
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  // Geschwindigkeit 150 von max. 255
  analogWrite(EnA, speed);
  // Motor B einschalten
  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
  // Geschwindigkeit 150 von max. 255
  analogWrite(EnB, speed);
  fstop();
}


void fstop() // Das Fahrzeug dreht so lange, bis der Flammensensor erkennt, dass er auf die Kerze ausgerichtet ist
{
  // while-Schleife bis Fahrzeug Grade Ausgerichtet ist
  while (fdirect[2] == 0) // Wenn Element 2 nicht mehr 0 ist, dann stoppt die wiederholung der while-Schleife
  {
    Serial.println("Ausrichtung");
    flammensensor();
  }

  // alle Motoren ausschalten
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
}

