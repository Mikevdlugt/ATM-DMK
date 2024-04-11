#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"

#define RX_PIN 5  //groen
#define TX_PIN 6  //blauw
//DTR_PIN  7  //geel
 
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);
 
unsigned int hoeveelheid_5 = 0;
unsigned int hoeveelheid_10 = 1;
unsigned int hoeveelheid_20 = 2;
unsigned int hoeveelheid_50 = 1;
unsigned int totaal5, totaal10, totaal20, totaal50, Totaal = 0;
unsigned int bonnetjesNummer_NaReset = 90000004;

unsigned long kaartNummer = 6393;
//datum var

/*void teller(){
  totaal5 = (hoeveelheid_5 * 5);
  totaal10 = (hoeveelheid_10 * 10);
  totaal20 = (hoeveelheid_20 * 20);
  totaal50 = (hoeveelheid_50 * 50);
  Totaal = (totaal5 + totaal10 + totaal20 + totaal50);
}void Printen(){
  printer.begin();
  delay(500);
  printer.upsideDownOn();
  printer.println("\n");
  //
  printer.boldOn();
  printer.setFont('A');
  printer.setSize('S');
  printer.justify('C');
  delay(200);
  printer.print("28 maart 2024");
  printer.println();
  // printer.boldOff();
  //
  // printer.boldOn();
  // printer.setFont('A');
  // printer.setSize('S');
  // printer.justify('C');
  delay(200);
  printer.print("Bon Nmr:  ");
  printer.print(bonnetjesNummer_NaReset);
  printer.println();
  printer.println();
  // printer.boldOff();
  //
  // printer.boldOn();
  // printer.setFont('A');
  // printer.setSize('S');
  // printer.justify('C');
  delay(200);
  printer.print(F("Kaart Nmr:  "));
  delay(200);
  printer.print("IMXX DMKB XXXXXX");
  delay(200);
  printer.print(kaartNummer);
  printer.println();
  // printer.boldOff();
  //
  // printer.boldOn();
  // printer.setFont('A');
  // printer.setSize('S');
  // printer.justify('C');
  delay(200);
  printer.print("Pin Totaal:          $");
  printer.print(Totaal);
  printer.print(",-");
  printer.println();
  printer.boldOff();
  //
  printer.println("--------------------------");
  //
  // printer.setFont('A'); //font-type(general)
  // printer.setSize('S'); //small
  // printer.justify('C');
  delay(200);
  printer.print(F("$50,-      "));
  printer.print(hoeveelheid_50);
  printer.print(" x         $");
  printer.print(totaal50);
  printer.print(",-");
  printer.println();
  //
  // printer.setFont('A'); //font-type(general)
  // printer.setSize('S'); //small
  // printer.justify('C');
  delay(200);
  printer.print(F("$20,-      "));
  printer.print(hoeveelheid_20);
  printer.print(" x         $");
  printer.print(totaal20);
  printer.print(",-");
  printer.println();
  //
  // printer.setFont('A'); //font-type(general)
  // printer.setSize('S'); //small
  // printer.justify('C');
  delay(200);
  printer.print(F("$10,-      "));
  printer.print(hoeveelheid_10);
  printer.print(" x         $");
  printer.print(totaal10);
  printer.print(",-");
  printer.println();
  //
  // printer.setFont('A'); //font-type(general)
  // printer.setSize('S'); //small
  // printer.justify('C');
  delay(200);
  printer.print(F("$5,-       "));
  printer.print(hoeveelheid_5);
  printer.print(" x         $");
  printer.print(totaal5);
  printer.print(",-");
  printer.println();
  //
  printer.println("Type:      Aantal:       Totaal:");
  //
  printer.setFont('B');
  printer.setSize('M');
  printer.justify('C');
  delay(200);
  printer.println("Pin Hoeveelheid: \n");
  //
  printer.setFont('A'); //Font-type(general)
  printer.setSize('M'); //Medium
  printer.justify('C');
  printer.underlineOn();  //underline
  delay(200);
  printer.println(F("       De Munten Kluis       "));
  //
  printer.boldOn();
  printer.setFont('A'); //Font-type(general)
  printer.setSize('L'); //Large
  printer.justify('C'); //general
  printer.underlineOff();  //underline
  delay(200);
  printer.println(F("DMK"));
  printer.println("\n\n");
  printer.boldOff();
}*/

void setup() {
  mySerial.begin(9600);
  /*teller();
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  Printen();*/
}

void loop() {
}