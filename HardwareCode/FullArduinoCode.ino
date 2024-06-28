#include <MFRC522.h>
#include <SPI.h>
#include <Keypad.h>
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"

//________Receipt Printer:
#define RX_PIN 46   // Recieve Pin, Green
#define TX_PIN 47   // Transmit Pin, Blue
#define DTR_PIN 48  // Reset Pin, Yellow  (Can Be Any Pin With An Low State And MUST Be Used)
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);
String date, last4IBANnumbers, transactionNum;
bool bonPrintenTF = false;

//________RFID-Reader:
#define RST_PIN 49               // Configurable Reset Pin
#define SS_PIN 53                // Configurable SDA Pin
MFRC522 rfid(SS_PIN, RST_PIN);   // Instance of the class
MFRC522::MIFARE_Key key;         //
byte readbackblock[18];          // This Array Is Used For Reading Out A Block. The MIFARE_Read method requires a buffer that is at least 18 bytes to hold the 16 bytes of a block.
int blockLength[2] = { 8, 10 };  // Length Of Block Thats Needed {Block X, Block Y}
bool returnVar;                  // Card Presence Check Flag

//________Buttons:
const int UserButtonArray[4] = { 22, 23, 24, 25 };  // Button Declaration For User (Front Buttons)
const int DevButtonArray[4] = { 42, 43, 44, 45 };   // Button Declaration For Developer (Back Buttons)
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
byte pin_rows[ROW_NUM] = { 2, 3, 4, 5 };                                              // Connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = { 6, 7, 8, 9 };                                         // Connect to the column pinouts of the keypad
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
String IBAN, ibanPrint;
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
  { 28, 29, 30, 0, 1 },  //motor 1   (pin +- / pin +- / select)
  { 31, 32, 33, 0, 1 },  //2
  { 34, 35, 36, 0, 1 },  //3
  { 37, 38, 39, 0, 1 }   //4
};
const int delaySwitch[2] = { 2200, 750 };

//________Serial Input/Output:
int geldPinValues[4] = { 0, 0, 0, 0 };                                    // Amount Of Money To Print
String incommingString_Sa;                                                // Serial Input Variable
int incommingChar;                                                        // Serial Input Variable
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

GeldToevoegen = 9 + 0/1/2/3 (type) + 1/5 (hoeveelheid)*/
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
/////////////////////// new file, same code:
bool kaartPresenceCheck() {                                            // if there is an card present
  returnVar = false;                                                   // reset value, this boolean keeps the programm alive while in use
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);       // reading out the uid
  for (short r = 0; r < 2; r++) {                                      // checking twice so there is no faulty check if the same card is presented twice
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {  // checks if there is an card present
      returnVar = true;
    }
  }
  return returnVar;
}
void CardIbanDefin() {
  rfid.PCD_Init();                     // start encryption on PCD , initialize the PCD pard of the rfid, for reading information of the card
  if (rfid.PICC_IsNewCardPresent()) {  // if PICC_IsNewCardPresent returns 1, a new card has been found and we continue
    if (rfid.PICC_ReadCardSerial()) {  // if PICC_ReadCardSerial returns 1, the "uid" struct (see MFRC522.h lines 238-45)) contains the ID of the read card.
      last4IBANnumbers = "";           // reseting iban string
      Serial.write("7");               // declaration char for the server to see the difference, of this iban
      int blockLength[2] = { 8, 10 };  // different block length, so there is no faulty extra bytes
      for (int block = 4; block <= 5; block++) {
        readBlock(block, readbackblock);                    // read the block back
        for (int j = 0; j < blockLength[(block - 4)]; j++)  // print the block contents
        {
          Serial.write(readbackblock[j]);  // Serial.write() transmits the ASCII numbers as human readable characters to serial monitor
          if (block == 5 && j >= 6) {
            last4IBANnumbers += (readbackblock[j], DEC);  // read the iban of in decimal numbers
          }
        }
      }
    }
  }
  rfid.PICC_HaltA();       // Halt PICC
  rfid.PCD_StopCrypto1();  // Stop encryption on PCD
}

int readBlock(int blockNumber, byte arrayAddress[]) {
  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3;  //determine trailer block for the sector

  /*****************************************authentication of the desired block for access***********************************************************/
  byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
  //byte PCD_Authenticate(byte command, byte blockAddr, MIFARE_Key *key, Uid *uid);
  //this method is used to authenticate a certain block for writing or reading
  //blockAddr is the number of the block from 0 to 15.
  //Uid *uid is a pointer to the UID struct that contains the user ID of the card.
  if (status != MFRC522::STATUS_OK) {
    // Serial.print("PCD_Authenticate() failed (read): ");
    // Serial.println(rfid.GetStatusCodeName(status));
    return 3;  //return "3" as error message
  }
  //If a different sector is being authenticated access to the previous one is lost.


  /*****************************************reading a block***********************************************************/

  byte buffersize = 18;
  status = rfid.MIFARE_Read(blockNumber, arrayAddress, &buffersize);  //&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
    // Serial.print("MIFARE_read() failed: ");
    // Serial.println(rfid.GetStatusCodeName(status));
    return 4;  //return "4" as error message
  }
  // Serial.println("block was read");
}

void CardUidDefin() {
  if (kaartPresenceCheck()) {
    uidString = "";
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
    SerialProcessing();
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
      Serial.print("99");
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
///////////////new file, same code:
void SerialProcessing() {
  while (Serial.available() > 0) {
    String incomingData = Serial.readString();
    switch (incomingData.charAt(0)) {
      case 'G':                         // if the incomming string starts With G, count money, print receipt if needed, dispense money
        for (int i = 1; i <= 4; i++) {  // read the values for each money type
          char data = incomingData.charAt(i);
          geldPinValues[(i - 1)] = ((int)data - 65);  // using ASCII to count the money : A=0, Z=25
        }
        delay(50);
        if (bonPrintenTF) {  // if a receipt needs to be printed
          moneyCount();      // count all money
          printenMain();     // print receipt
        }
        delay(50);
        dispenseMoney();  // Start Dispensing Money
        break;
      case 'D':                                                 // reading out the Date and Time (DD/MM/YYYY HH:MM:SS), the last 4 numbers of the Iban, and the Transaction number. (all in 1 string)
        date = "", last4IBANnumbers = "", transactionNum = "";  // reset values
        for (int i = 1; i <= 19; i++) {
          char data = incomingData.charAt(i);  //reading char per char
          if (data == '/') {
            date += "/";
          } else if (data == ':') {
            date += ":";
          } else if (data == ' ') {
            date += " ";
          } else if (data > 47 && data < 58) {
            date += data;
          }
        }
        for (short i = 20; i <= 23; i++) {  // reading out the Iban number
          last4IBANnumbers += incomingData.charAt(i);
        }
        for (short i = (incomingData.length() - (incomingData.length() - 23)); i < incomingData.length(); i++) {  //reading out the Transaction number
          transactionNum += incomingData.charAt(i);
        }
        transactionNum.toInt();
        bonPrintenTF = true;
        break;
      case 'C':                // Reset the full code, go to the start, so it can be run agin
        returnVar = false;     // flag, which keeps the code alive
        bonPrintenTF = false;  // reset receipt flag
        break;
      default:
        Serial.print("Unknown comman: " + incomingData.charAt(0));
        break;
    }
  }
}
/////////////////new file, same code:
void dispenseMoney() {
  for (short g = 0; g < 4; g++) {  // switching between all 4 money values
    if (geldPinValues[g]) {        // if value is 1 or greater
      geldPinValues[g]++;          // increment it with one, because moving the motors backwards is already 1 loop, and it will decrement while nothing really heapend. so increment with 1 so that it will turn correct amounts
    }
  }
  while (geldPinValues[0] || geldPinValues[1] || geldPinValues[2] || geldPinValues[3]) {  // while the motors need to move. even if its only 1 motor
    for (int delaySwitchVar = 0; delaySwitchVar < 2; delaySwitchVar++) {                  // One Start/stop per iteration. Switching between forward and back, with forward an longer delay.
      startStopMotoren(1);                                                                // start motor
      delay(delaySwitch[delaySwitchVar]);                                                 // desired delay
      startStopMotoren(0);                                                                // stop motor
      pinsInvertenEnInstellen();                                                          // switch rotation
      delay(500);
    }
    for (short geldverminderen = 0; geldverminderen < 4; geldverminderen++) {  // switching between all 4 motors
      if (geldPinValues[geldverminderen]) {                                    // if the motor needs to move / there is an value higher than 0
        geldPinValues[geldverminderen]--;                                      // decrement that value
      }
    }
  }
}

void startStopMotoren(short f) {             // start or stop the motors, depending on the value in the parameter
  for (int motor = 0; motor < 4; motor++) {  // switching between all 4 motors
    if (geldPinValues[motor] > 0) {          // if the motor needs to move
      digitalWrite(motorpins[motor][2], f);  // Start/stop motor, with a 1(HIGH) or 0(LOW);
    }
  }
}

void pinsInvertenEnInstellen() {                                                         // switch rotation
  for (int motor = 0; motor < 4; motor++) {                                              // motor select
    if (geldPinValues[motor] > 0) {                                                      // if the motor needs to move
      for (int motorFlags = 3; motorFlags <= 4; motorFlags++) {                          // switching between forward and backwards rotation
        motorpins[motor][motorFlags] = !motorpins[motor][motorFlags];                    // switch the values of the array, where the pin values are (1 or 0 value), so that the motors will switch rotation
        digitalWrite(motorpins[motor][(motorFlags - 3)], motorpins[motor][motorFlags]);  // setting pins to the desired changed values
      }
    }
  }
}
///////////////////new file, same code:
void printenMain() {
  mySerial.begin(9600);        // extra Serial line to the receipt printer
  pinMode(DTR_PIN, OUTPUT);    // necessary output pin
  digitalWrite(DTR_PIN, LOW);  // this will fix faulty receipts, this returns faulty current and acts like a GROUND
  Printen();
  receiptNumber_AfterReset += 1;  // receiptnumber, incerementing after each use. depending on the last reset
}

void moneyCount() {                                                                                          // calculate the total amount of each valued money type, and the total amount of all valued money types together
  totalMoneyValue[1] = (geldPinValues[0] * 5);                                                               // total of 5 pounds
  totalMoneyValue[2] = (geldPinValues[1] * 10);                                                              // total of 10 pounds
  totalMoneyValue[3] = (geldPinValues[2] * 20);                                                              // total of 20 pounds
  totalMoneyValue[4] = (geldPinValues[3] * 50);                                                              // total of 50 pounds
  totalMoneyValue[0] = (totalMoneyValue[1] + totalMoneyValue[2] + totalMoneyValue[3] + totalMoneyValue[4]);  // full total moneyff
}

void Printen() {  // Receipt printer print lines
  printer.begin();
  delay(200);
  printer.upsideDownOn();
  delay(50);
  printer.boldOn();
  printer.setFont('A');  //font-type(general) of B
  printer.setSize('S');  //small of M(edium) of L(arge)
  printer.justify('C');  //Center
  delay(50);
  printer.println("\n");
  printer.println(date);
  //
  delay(5);
  printer.print("Bon Nmr:  ");
  printer.println(receiptNumber_AfterReset);
  //
  delay(5);
  printer.println("= = = = = = = = = = = = = = = =");
  //
  delay(5);
  printer.print("TransactieNum: ");
  printer.println((transactionNum = +1));
  //
  delay(5);
  printer.print("Kaart Nmr: ");
  printer.print("IMXX DMKB XXXXXX");
  printer.println(last4IBANnumbers);  //last4IBANnumbers
  //
  delay(5);
  printer.print("\nPin Totaal:          $");
  printer.print(totalMoneyValue[0]);
  printer.println(",-");
  //
  delay(5);
  printer.println("= = = = = = = = = = = = = = = =");
  //
  delay(5);
  printer.print("$50,-      ");
  printer.print(geldPinValues[3]);
  printer.print("x         $");
  printer.print(totalMoneyValue[4]);
  printer.println(",-");
  //
  delay(5);
  printer.print("$20,-      ");
  printer.print(geldPinValues[2]);
  printer.print("x         $");
  printer.print(totalMoneyValue[3]);
  printer.println(",-");
  //
  delay(5);
  printer.print("$10,-      ");
  printer.print(geldPinValues[1]);
  printer.print("x         $");
  printer.print(totalMoneyValue[2]);
  printer.print(",-");
  printer.println();
  //
  delay(5);
  printer.print("$5,-       ");
  printer.print(geldPinValues[0]);
  printer.print("x         $");
  printer.print(totalMoneyValue[1]);
  printer.println(",-");
  //
  delay(5);
  printer.println("= = = = = = = = = = = = = = = =");
  //
  delay(5);
  printer.println("Type:      Aantal:       Totaal:");
  //
  printer.setFont('B');
  printer.setSize('M');
  printer.justify('C');
  delay(5);
  printer.println("Pin Hoeveelheid:\n");
  //
  printer.setFont('A');  //Font-type(general)
  delay(5);
  printer.println(F("       De Munten Kluis       "));
  //
  printer.setSize('L');  //Large
  delay(200);
  printer.println(F("DMK"));
  printer.println("\n");
  printer.println("\n");
  printer.boldOff();
}
