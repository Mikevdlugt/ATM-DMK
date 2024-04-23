// 24-4-2024 01:55AM  |  Kai van der Knijff
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

#define RST_PIN 9
#define SS_PIN 10
const int ButtonArray[4] = { 4, 5, 6, 7 };  // Button Declaration/Initialisation

// rfid (Radio-Frequentie Identification)
MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class
MFRC522::MIFARE_Key key;
// byte nuidPICC[4];

//Keypad constant/static variables
const int ROW_NUM = 4;              // four rows
const int COLUMN_NUM = 4;           // three columns
char keys[ROW_NUM][COLUMN_NUM] = {  // Keypad Map
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte pin_rows[ROW_NUM] = { 38, 37, 36, 35 };       // connect to the row pinouts of the keypad    // temporary random pin values
byte pin_column[COLUMN_NUM] = { 34, 33, 32, 30 };  // connect to the column pinouts of the keypad // temporary random pin values
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// Usable button variables
char keypadButton;

// Pre-writen recognition variables. (Array[0] = Unique Group Identifier)
char keyPrint[2] = { '5', ' ' };  // 5 = key recognition Identifier (51 = key: 1 | 5# = key: #)
//char uid
//char iban

void setup() {
  Serial.begin(9600);
  SPI.begin();      // Begin SPI connection
  rfid.PCD_Init();  // Initialize function to manipulate rfid

  for (int i = 0; i < 4; i++) {
    pinMode(ButtonArray[i], INPUT_PULLUP);  // Initialization of 4 buttons
  }
}

void loop() {

  if (kaartPresenceCheck()) {
    Serial.print("IM69 DMKB0123456789");  //Serial.print(ibanKaartPrint());
    Serial.print("uid 1A2B3C4D");         //Serial.print(uidKaartPrint());
    delay(1000);

    awaitConfirmation();  // Awaiting for confirmation to continue

    while (kaartPresenceCheck()) {
      // Serial.print(".");
      keypadButton = keypad.getKey();

      if (keypadButton) {  // Waiting/checking for keypad input
        keyPrint[1] = keypadButton;
        Serial.println(keyPrint);
      }

      for (int i = 0; i < 4; i++) {  // Waiting/checking for
        if (digitalRead(ButtonArray[i]) == LOW) {
          // Serial.print("CLICKED BUTTON ");
          Serial.println((i + 1));
        }
      }
    }
  }
  // Serial.print(",");
}
//---------------------PageDevider
bool returnVar;
bool kaartPresenceCheck() {
  returnVar = NULL;
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    returnVar = true;
  }
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    returnVar = true;
  }
  return returnVar;
}

bool infLoopPrevent;
char cfrmInput;
void awaitConfirmation() {
  infLoopPrevent = false;
  while (kaartPresenceCheck() && !infLoopPrevent) {
    if (Serial.available() > 1) {
      cfrmInput = Serial.read();
      cfrmInput = Serial.read();
      if (cfrmInput == '1') {
        Serial.println("Serial Read Correct");
        infLoopPrevent = true;
        cfrmInput = '0';
      }
    }
  }
}
