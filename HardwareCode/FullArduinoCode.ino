#include <MFRC522.h>
#include <SPI.h>
#include <Keypad.h>
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"

//________Receipt Printer:
#define RX_PIN 38    // Recieve Pin, Green
#define TX_PIN 39  // Transmit Pin, Blue
//#define DTR_PIN *  // Reset Pin, Yellow  (Can Be Any Pin With An Low State And MUST Be Used)
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);
String date = "", time = " ";

//________RFID-Reader:
#define RST_PIN 49               // Configurable Reset Pin
#define SS_PIN 53                // Configurable SDA Pin
MFRC522 rfid(SS_PIN, RST_PIN);   // Instance of the class
MFRC522::MIFARE_Key key;         //
byte readbackblock[18];          // This Array Is Used For Reading Out A Block. The MIFARE_Read method requires a buffer that is at least 18 bytes to hold the 16 bytes of a block.
int blockLength[2] = { 8, 10 };  // Length Of Block Thats Needed {Block X, Block Y}
bool returnVar;                  // Card Presence Check Flag

//________Buttons:
const int UserButtonArray[4] = { 30, 31, 32, 33 };  // Button Declaration For User (Front Buttons)
const int DevButtonArray[4] = { 34, 35, 36, 37 };   // Button Declaration For Developer (Back Buttons)
bool DevDoubleKeyPress[4], UserDoubleKeyPress[4];   // Button Flags (True/False)

//________Keypad:
const int ROW_NUM = 4, COLUMN_NUM = 4;  // Four rows & Four columns
char keypadButton,                      // Keypad Button Input
  keys[ROW_NUM][COLUMN_NUM] = {         // Keypad Map
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' }
  };
byte pin_rows[ROW_NUM] = { 22, 23, 24, 25 };                                          // Connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = { 26, 27, 28, 29 };                                     // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);  //
// char keypadButton;

//________Developer Display:
const int displayPins[9] = { A0, A1, A2, A3, A4, A5, A6, A7, A8 },  // Display Pins
  displaySegments[5][8] =                                           // Display Segment Declaration
  {
    { 0, 150, 150, 150, 150, 150, 150, 0 },  // 5 (With Dot)
    { 150, 0, 0, 0, 0, 150, 0, 0 },          // 1(0)
    { 150, 150, 0, 0, 150, 0, 150, 150 },    // 2(0)
    { 0, 150, 150, 0, 150, 150, 150, 0 },    // 5(0)
    { 0, 0, 0, 0, 0, 0, 0, 0 }               // OFF
  };

//________IBAN & UID:
char tempPartIban;
String IBAN, ibanPrint, last4IBANnumbers;
byte IBANasciiValue[18];
String uidString, uidByteLengthCheck, uidByteFix = "0";  // Check If UID-Byte == 2  /
bool devUFlag = true;

//________Money:
short devMoneyTypeSelect = 0;                          // Starting At 5€
char addMoneyVirtualStorageID[3] = { '1', '2', '3' };  // +1 / +10 / -1
String moneyAddID;                                     // Printable String For Server Code For Knowing The Amount Of Money In The Physical ATM
unsigned int totalMoneyValue[5] = { 0, 0, 0, 0, 0 },   // Total | 5 | 10 | 20 | 50
  receiptNumber_AfterReset = 1;

//________MoneyDispenser:
int motorpins[4][5] = {
  { 11, 12, 13, 0, 1 },  //motor 1
  { 8, 9, 10, 0, 1 },    //2
  { 5, 6, 7, 0, 1 },     //3
  { 2, 3, 4, 0, 1 }      //4
};
const int delaySwitch[2] = { 1600, 650 };

//________Serial Input/Output:
int geldPinValues[4] = { 0, 0, 0, 0 };                                    // Amount Of Money To Print
int incommingByte;                                                        // Serial Input Variable
char incommingChar;                                                       // Serial Input Variable
String keypadChar = "5", uidChar = "6", ibanChar = "7", moneyChar = "9";  // Variables For Identification

/*OUTPUT Explaination:
GeenKaartAanwezig = 0;
Button 1          = 1
Button 2          = 2
Button 3          = 3
Button 4          = 4
KeypadButtons     = 5 + button
UID               = 6 + UID
IBAN              = 7 + IBAN

GeldToevoegen = 9 + 0/1/2/3 (type) + 1/2/3 (+1/+10/-1)*/
////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  SPI.begin();      // Begin SPI Connection
  rfid.PCD_Init();  // Initialize Function To Manipulate RFID

  for (byte i = 0; i < 6; i++) {  //
    key.keyByte[i] = 0xFF;        // keyByte Is Defined In The "MIFARE_Key" 'struct' Definition In The .h File Of The Library
  }
  for (int i = 0; i < 4; i++) {                                             // Initialization Of:
    pinMode(UserButtonArray[i], INPUT_PULLUP);                              //- 4 USER (-Interface) buttons
    pinMode(DevButtonArray[i], INPUT_PULLUP);                               //- 4 Developer Buttons
    UserDoubleKeyPress[i] = true;                                           //- 4 USER (-Interface) Button Flags
    DevDoubleKeyPress[i] = true;                                            //- 4 Developer Button Flags
  }                                                                         //
  for (int i = 0; i < sizeof(displayPins) / sizeof(displayPins[0]); i++) {  //- 9 Display Segment Pins
    pinMode(displayPins[i], OUTPUT);
  }
  for (int motor = 0; motor < 4; motor++) {                // Initialization of all 12 motors pins; Incrementing Through All 4 Motors
    for (int motor_pin = 0; motor_pin < 3; motor_pin++) {  // Incrementing Through All 3 Pins Ech Motor
      pinMode(motorpins[motor][motor_pin], OUTPUT);        // Settings Pins As Output
    }                                                      //
    digitalWrite(motorpins[motor][2], LOW);                // Setting All Select Pins Low, So None Of The Motors Will Start To Spin At Setup
  }
}

void loop() {
  if (kaartPresenceCheck()) {
    CardUidDefin();             // Request And Process UID
    if (!devUFlag) {            //
      CardIbanDefin();          // Request And Process IBAN
      Serial.print(ibanPrint);  // Print IBAN
      Serial.print(uidString);  // Print UID
      delay(1000);
      UserButtonPressing();
    } else if (devUFlag) {
      DevButtonPressing();
    }
  }
  returnVar = false;
}

////////// Next File:

bool kaartPresenceCheck() {
  returnVar = false;
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  for (short r = 0; r < 2; r++) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      returnVar = true;
    }
  }
  return returnVar;
}
void CardIbanDefin() {
  rfid.PCD_Init();
  if (rfid.PICC_IsNewCardPresent()) {  //if PICC_IsNewCardPresent returns 1, a new card has been found and we continue
    if (rfid.PICC_ReadCardSerial()) {  //if PICC_ReadCardSerial returns 1, the "uid" struct (see MFRC522.h lines 238-45)) contains the ID of the read card.

      Serial.write("7");
      int blockLength[2] = { 8, 10 };
      for (int block = 4; block <= 5; block++) {
        readBlock(block, readbackblock);                    //read the block back
        for (int j = 0; j < blockLength[(block - 4)]; j++)  //print the block contents
        {
          Serial.write(readbackblock[j]);  //Serial.write() transmits the ASCII numbers as human readable characters to serial monitor
        }
      }
    }
  }
  rfid.PICC_HaltA();       // Halt PICC
  rfid.PCD_StopCrypto1();  // Stop encryption on PCD
  // ////////////////////////////////////////////
  // if (kaartPresenceCheck()) {

  //   IBAN = "IM67DMKB8194650327";
  //   // for (int c = 14; c < 18; c++) {
  //   //   last4IBANnumbers += IBAN.charAt(c);
  //   // }
  //   ibanPrint = ibanChar + IBAN;
  // }
}

int readBlock(int blockNumber, byte arrayAddress[]) {
  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3;  //determine trailer block for the sector

  /*****************************************authentication of the desired block for access***********************************************************/
  byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
  //byte PCD_Authenticate(byte command, byte blockAddr, MIFARE_Key *key, Uid *uid);
  //this method is used to authenticate a certain block for writing or reading
  //command: See enumerations above -> PICC_CMD_MF_AUTH_KEY_A	= 0x60 (=1100000),		// this command performs authentication with Key A
  //blockAddr is the number of the block from 0 to 15.
  //MIFARE_Key *key is a pointer to the MIFARE_Key struct defined above, this struct needs to be defined for each block. New cards have all A/B= FF FF FF FF FF FF
  //Uid *uid is a pointer to the UID struct that contains the user ID of the card.
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed (read): ");
    Serial.println(rfid.GetStatusCodeName(status));
    return 3;  //return "3" as error message
  }
  //it appears the authentication needs to be made before every block read/write within a specific sector.
  //If a different sector is being authenticated access to the previous one is lost.


  /*****************************************reading a block***********************************************************/

  byte buffersize = 18;                                               //we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size...
  status = rfid.MIFARE_Read(blockNumber, arrayAddress, &buffersize);  //&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_read() failed: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return 4;  //return "4" as error message
  }
  // Serial.println("block was read");
}

void CardUidDefin() {
  if (kaartPresenceCheck()) {
    uidString = "";
    // uidString = uidChar;
    for (short u = 0; u < 4; u++) {
      uidByteLengthCheck = String(rfid.uid.uidByte[u], HEX);
      if (uidByteLengthCheck.length() == 1) {
        uidByteLengthCheck = uidByteFix + uidByteLengthCheck;
      }
      uidString += uidByteLengthCheck;
    }
    uidString.toUpperCase();
  }
  String DeveloperUID_UID = "D0FFA021";
  devUFlag = true;
  for (short devU = 1; devU < 8; devU++) {
    if (uidString.charAt(devU) != DeveloperUID_UID.charAt(devU)) {
      devUFlag = false;
      // Serial.println("FouteUID");
      break;
    }
  }
}

void UserButtonPressing() {
  for (short p = 0; p < 4; p++) {
    UserDoubleKeyPress[p] = true;
  }
  while (returnVar) {
    keypadButton = keypad.getKey();

    if (keypadButton) {  //checking for keypad input
      Serial.print(keypadChar + String(keypadButton));
    }

    for (short i = 0; i < 4; i++) {  //checking all 4 buttons if pressed
      if ((digitalRead(UserButtonArray[i]) == LOW) && UserDoubleKeyPress[i]) {
        Serial.print((i + 1));
        UserDoubleKeyPress[i] = false;
      } else if (digitalRead(UserButtonArray[i]) == HIGH) {
        UserDoubleKeyPress[i] = true;
      }
    }

    if (Serial.available() > 0) {
      SerialProcessing();
    }
  }
}

void DevButtonPressing() {
  while (returnVar) {
    for (short i = 0; i < 4; i++) {  //checking all 4 buttons if pressed
      if ((digitalRead(DevButtonArray[i]) == LOW) && DevDoubleKeyPress[i]) {
        DevDoubleKeyPress[i] = false;
        DevStorageInput(i);
      } else if (digitalRead(DevButtonArray[i]) == HIGH) {
        DevDoubleKeyPress[i] = true;
      }
    }
    keypadButton = keypad.getKey();

    if (keypadButton == '*') {  //checking for keypad input
      returnVar = false;
    }
  }
}

void DevStorageInput(short i) {
  switch (i) {
    case 0:
      geldInputPrint(i);
      break;
    case 1:
      geldInputPrint(i);
      break;
    case 2:
      geldInputPrint(i);
      break;
    case 3:
      if (devMoneyTypeSelect != 3) {
        devMoneyTypeSelect++;
      } else {
        devMoneyTypeSelect = 0;
      }

      for (int pinss = 0; pinss < 8; pinss++) {
        analogWrite(displayPins[pinss], displaySegments[devMoneyTypeSelect][pinss]);
      }
      if (devMoneyTypeSelect != 0) {
        analogWrite(displayPins[8], 150);
      } else {
        analogWrite(displayPins[8], 0);
      }
      delay(750);
      for (int pinss = 0; pinss < 8; pinss++) {
        analogWrite(displayPins[pinss], displaySegments[4][pinss]);
        analogWrite(displayPins[8], 0);
      }
      break;
  }
}

void geldInputPrint(short j) {
  moneyAddID = moneyChar + devMoneyTypeSelect + addMoneyVirtualStorageID[j];
  Serial.print(moneyAddID);
}

////////// Next File:

void SerialProcessing() {
  while (Serial.available() > 0) {
    incommingByte = Serial.read();

    if (incommingByte == 71) {  //een hoeveelheid dispensen + printen
      Serial.println("test");
      for (short p = 0; p < 4; p++) {
        geldPinValues[p] = 0;
      }
      while (Serial.available() > 0) {
        incommingByte = Serial.read();
        geldPinValues[(incommingByte - 65)]++;
        Serial.println(incommingByte);
      }
      dispenseMoney();
      // printenMain();
      // returnVar = false;
    }

    if (incommingByte == 67) {
      returnVar = false;
      incommingByte = Serial.read();
      return;
    }

    if (incommingByte == 68) {
      date = "";
      while (Serial.available() > 0) {
        incommingByte = Serial.read();

        if (incommingByte == 47) {
          date += String("/");
        } else if (incommingByte == 58) {
          date += String(":");
        } else if (incommingByte == 32) {
          date += String(" ");
        } else if (incommingByte != 10) {
          date += String((incommingByte - 48));
        }
      }
      return;
    }
  }
}

////////// Next File:

void dispenseMoney() {
  Serial.println("dispenseMoney");
  for (short moneyCorrection = 0; moneyCorrection < 4; moneyCorrection++) {
    // geldPinValues[moneyCorrection]++;
    Serial.println(geldPinValues[moneyCorrection]);
  }
  while ((geldPinValues[0] + geldPinValues[1] + geldPinValues[2] + geldPinValues[3]) > 0) {
    Serial.println("\n============");  //s
    for (int f = 0; f < 4; f++) {      //s
      Serial.print(geldPinValues[f]);  //s
      Serial.print(" | ");             //s
    }                                  //s
    Serial.print("\n============");    //s

    for (int delaySwitchVar = 0; delaySwitchVar < 2; delaySwitchVar++) {
      startStopMotoren(1);
      delay(delaySwitch[delaySwitchVar]);
      startStopMotoren(0);
      pinsInvertenEnInstellen();
      delay(500);
    }
    for (short geldverminderen = 0; geldverminderen < 4; geldverminderen++) {
      if (geldPinValues[geldverminderen] > 0) {
        geldPinValues[geldverminderen] -= 1;
      }
    }
  }
}

void startStopMotoren(short f) {
  Serial.print("\nStart/Stop motoren:   ");  //s
  for (int motor = 0; motor < 4; motor++) {
    if (geldPinValues[motor] > 0) {
      digitalWrite(motorpins[motor][2], f);  // Start motor
      Serial.print((motor + 1));             //s
    }
  }
}

void pinsInvertenEnInstellen() {
  for (int motor = 0; motor < 4; motor++) {  // motor select
    if (geldPinValues[motor] > 0) {
      for (int motorFlags = 3; motorFlags <= 4; motorFlags++) {                          //wisselen tussen vooruit en achteruit
        motorpins[motor][motorFlags] = !motorpins[motor][motorFlags];                    //
        digitalWrite(motorpins[motor][(motorFlags - 3)], motorpins[motor][motorFlags]);  //pins intellen
      }
    }
  }
}

////////// Next File:

void printenMain() {
  mySerial.begin(9600);
  moneyCount();
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  Printen();
  if (true) receiptNumber_AfterReset += 1;
}

void moneyCount() {
  totalMoneyValue[1] = (geldPinValues[0] * 5);
  totalMoneyValue[2] = (geldPinValues[1] * 10);
  totalMoneyValue[3] = (geldPinValues[2] * 20);
  totalMoneyValue[4] = (geldPinValues[3] * 50);
  totalMoneyValue[0] = (totalMoneyValue[1] + totalMoneyValue[2] + totalMoneyValue[3] + totalMoneyValue[4]);
}

void Printen() {
  printer.begin();
  delay(500);
  printer.upsideDownOn();
  printer.boldOn();
  printer.setFont('A');  //font-type(general) of B
  printer.setSize('S');  //small of M(edium) of L(arge)
  printer.justify('C');  //Center
  delay(200);
  printer.println("\n");
  printer.println(date);
  //
  delay(200);
  printer.print("Bon Nmr:  ");
  printer.println(receiptNumber_AfterReset);
  //
  delay(200);
  printer.println("= = = = = = = = = = = = = = = =");
  //
  delay(200);
  printer.print("Kaart Nmr: ");
  printer.print("IMXX DMKB XXXXXX");
  printer.println(last4IBANnumbers);
  //
  delay(200);
  printer.print("Pin Totaal:          $");
  printer.print(totalMoneyValue[0]);
  printer.println(",-");
  //
  delay(200);
  printer.println("= = = = = = = = = = = = = = = =");
  //
  delay(200);
  printer.print("$50,-      ");
  printer.print(geldPinValues[3]);
  printer.print("x         $");
  printer.print(totalMoneyValue[4]);
  printer.println(",-");
  //
  delay(200);
  printer.print("$20,-      ");
  printer.print(geldPinValues[2]);
  printer.print("x         $");
  printer.print(totalMoneyValue[3]);
  printer.println(",-");
  //
  delay(200);
  printer.print("$10,-      ");
  printer.print(geldPinValues[1]);
  printer.print("x         $");
  printer.print(totalMoneyValue[2]);
  printer.print(",-");
  printer.println();
  //
  delay(200);
  printer.print("$5,-       ");
  printer.print(geldPinValues[0]);
  printer.print("x         $");
  printer.print(totalMoneyValue[1]);
  printer.println(",-");
  //
  delay(200);
  printer.println("= = = = = = = = = = = = = = = =");
  //
  delay(200);
  printer.println("Type:      Aantal:       Totaal:");
  //
  printer.setFont('B');
  printer.setSize('M');
  printer.justify('C');
  delay(200);
  printer.println("Pin Hoeveelheid:\n");
  //
  printer.setFont('A');  //Font-type(general)
  delay(200);
  printer.println(F("       De Munten Kluis       "));
  //
  printer.setSize('L');  //Large
  delay(200);
  printer.println(F("DMK"));
  printer.println("\n\n");
  printer.boldOff();
  // Serial.println("klaar met printen");
}
