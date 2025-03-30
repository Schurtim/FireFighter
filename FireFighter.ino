#define EnA 6
#define EnB 5
#define In1 13
#define In2 12
#define In3 11
#define In4 10
#define echo 3
#define trigger 4

//Geschwindigkeitsvariablen
int speed = 180;   // Drehgeschwindigkeit; max. 255
int vspeed = 100;  // Vorwärtsgeschwindigkeit; max. 255

int pins[5] = { A1, A2, A3, A4, A5 };  // Pins des Flammensensors zum Auslesen
int fwert[5] = { 0, 0, 0, 0, 0 };      // Analogwerte des Flammensensors der einzelnen Pins
int fdirect[5] = { 0, 0, 0, 0, 0 };    // flammensensor der Richtung und Angabe in 0 oder 1; nur eine 1 möglich

long entfernung = 0; // Variable zum speichern der Entfernung
long zeit = 0; // Variable zum Speichern der Zeit des Ultraschallsensors


void setup() 
{
  // Starte Serielle Ausgabe
  Serial.begin(9600);

  // deklariere Analoge-Pins 1-5 als INPUT
  for (int i = 0; i < 5; i++)
  {
    pinMode(pins[i], INPUT);
  }

  // Pins als Output/Input deklarieren
  pinMode(EnA, OUTPUT);
  pinMode(EnB, OUTPUT);
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(In3, OUTPUT);
  pinMode(In4, OUTPUT);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  delay(3000);
}



void loop() 
{
  flammensensor();
  ausrichtung();
  ultraschallsensor();
  loeschung();
}


void flammensensor() 
{
  int fdirectIndex = 0;  // Index des maximalen Werts
  int fdirectValue = 0;  // maximaler Wert

  for (int i = 0; i < 5; i++) 
  {
    fwert[i] = analogRead(pins[i]);
    Serial.print("A");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(fwert[i]);

    // Überprüfen, ob der aktuelle Wert größer als der bisherige maximalwert ist
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

  // Setze den Maximalwert auf 1 und alle anderen Werte auf 0
  for (int i = 0; i < 5; i++) {
    fdirect[i] = (i == fdirectIndex) ? 1 : 0;
  }
}


void ausrichtung()
{
  // ausrichtung mithilfe von fdirect ausführen
  for (int i = 0; i < 5; i++) {
    if (fdirect[i] == 1) {
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
  delay(2000);
}


void ultraschallsensor()
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
  loeschung();
}


void loeschung()
{
  // alle Motoren ausschalten
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
  // starte Löschung
  Serial.println("Löschung");
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
void links()  //Motor A dreht vorwärts, Motor B dreht rückwärts
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
  stop();
}
void rechts()  //Motor A dreht rückwärts, Motor B dreht vorwärts
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
  stop();
}


void stop() {
  // while-Schleife bis Fahrzeug Grade Ausgerichtet ist
  while (fdirect[2] == 0) {
    delay(20);
    Serial.println("Ausrichtung");
    flammensensor();
  }
/*
  // alle Motoren ausschalten
  digitalWrite(In1, LOW);
  digitalWrite(In2, LOW);
  digitalWrite(In3, LOW);
  digitalWrite(In4, LOW);
  Serial.println("Stop");
  */
}