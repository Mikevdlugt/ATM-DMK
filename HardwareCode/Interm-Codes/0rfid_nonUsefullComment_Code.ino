//OK klikken
//fixen, dat wanneer er voor verificatie de pas NOG een keer gevraagd wordt, en er wordt een andere kaart geplaatst, dat ie dan zegt: wrong card, use your original card ofzo
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#define SS_PIN 5
#define RST_PIN 0
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
byte nuidPICC[4];// Init array that will store new NUID 

bool pin_loopValue = true;
bool kaart_loopValue = false;
bool pin_Database_check = false;
bool databaseGoeieValueTerug = false;
bool valueBack12345;//////
bool newCard = false;

short pinStrikes = 0;
short maxPinStrikes = 3;
char pinCode[4] = {0, 0, 0, 0};

const short ROW_NUM = 4; //four rows
const short COLUMN_NUM = 4; //three columns
char keys[ROW_NUM][COLUMN_NUM] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
byte pin_rows[ROW_NUM] = {32, 33, 25, 26}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {27, 14, 12, 13}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

void setup() { 
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }//Serial.println(F("This code scan the MIFARE Classsic NUID \n Using the following key:")); printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
void printPin(){for (int x = 0; x < sizeof(pinCode); x++){Serial.print(pinCode[x]);}}
void loop() {
  if(kaartCheck() == false){
    return;
  }
  Serial.println("\nWelkom bij deze kut ATM, volgend scherm**");
  if (pincodeInvoeren() == false){
    return;
  }
  Serial.println("\nPlease put your card in again to verify (unless its already in) \n Waiting for card");
  do{
    Serial.print(".");
  }while(! rfid.PICC_IsNewCardPresent() && (rfid.uid.uidByte[0] == nuidPICC[0] && rfid.uid.uidByte[1] == nuidPICC[1] && rfid.uid.uidByte[2] == nuidPICC[2] && rfid.uid.uidByte[3] == nuidPICC[3]));
  pincodeInvoeren();
}

bool kaartCheck(){
  if ( ! rfid.PICC_IsNewCardPresent() || kaart_loopValue == false){  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    kaart_loopValue = true;
    return false;
  }
  if ( ! rfid.PICC_ReadCardSerial()){  // Verify if the NUID has been readed
    return false;
  }
  //Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  //Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && 
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return false;
  }
  newCardCheckWithPrevious();
  return true;
}

bool newCardCheckWithPrevious(){
  //PRINTING UID: (HEX & DEC) + check dubble card read
  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] ) {  //checkt of uid niuew is
    // Serial.println(F("A new card has been detected."));
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    rfid.PICC_HaltA();//HALT PICC
    rfid.PCD_StopCrypto1();// Stop encryption on PCD
    return true;
  }
  else{
    // Serial.println(F("Card read previously."));
    rfid.PICC_HaltA();//HALT PICC
    rfid.PCD_StopCrypto1();// Stop encryption on PCD
    return false;
  }
}

bool pincodeInvoeren(){
  pinStrikes = 0;
  pin_Database_check = false;
  while(pin_Database_check == false && pinStrikes < maxPinStrikes){ //while valueBack from database is false (pin not true)

    //key presses in array zetten
    Serial.print("\nvoer uw pincode in:   ");
    for(int i = 0; i < sizeof(pinCode); i++){
      while(pin_loopValue == true){ //wachten tot keypress
        char key = keypad.getKey();
        
        // if(key){  //if key is pressed
        if(key == 'C'){
          if(i != 0){
            Serial.println("\n previous number has been deleted\n");
            pinCode[(i-1)] = 0;
            i--;
          }else{
            Serial.println("\nCant DELETE IF THERE IS NONE\n");
          }
          for (int pprint = 0; pprint < sizeof(pinCode); pprint++) {
            if(pinCode[pprint] != 0){
              Serial.print("*");
            }
          }
        }else if(key){
          Serial.print("*");
          pinCode[i] = key;
          pin_loopValue = false;
        }
      }
      pin_loopValue = true; //reset while loop for next iteration
    }
    valueBack12345, databaseGoeieValueTerug = false;
    while(databaseGoeieValueTerug == false){
      pinCheckDatabase();
      pin_Database_check = valueBack12345;
      if (pin_Database_check == true){//databaseGoeieValueTerug, if=true/ check ging goed (ookal kan pin niet juist)
        Serial.println("\nPIN WAS GOED OFZO");
        pinStrikes = 0;
        databaseGoeieValueTerug = true;
      } else if (pin_Database_check == false){
        Serial.println("\nPIN IS FOUT OFZO, PROBEER OPNIEUW");
        pinStrikes++;
        databaseGoeieValueTerug = true;
      } else {
        Serial.println("\nSOMETHING WENT WRONG ASKING SERVER FOR VERIFICATION, WE WILL TRY AGAIN");
        //delay(200);
      }
    }
  }
  bool eindPinBoolReturn;
  if(pinStrikes >= maxPinStrikes){
    Serial.println("\npincode is tevaak verkeerd ingevoerd, crash alles");
    eindPinBoolReturn = false;
  }else if(pin_Database_check == true){
    eindPinBoolReturn = true;
  }
  return eindPinBoolReturn;
}

void pinCheckDatabase(){  //checked of de pin klopt in de database
  // for(int j = 0; j < sizeof(pinCode); j++){    //hier moet een functie komen om met database 1 voor 1 de data door te sturen. rfid.uid.uidByte(,rfid.uid.size) en pin
  //   Serial.print(pinCode[j]);                  //en dat dan in een nieuwe functie.
  //   Serial.pint("PINCODE IS GECHECKED");}
  if(pinCode[0] == '9'){   //als valueBack from database is true (pin not true), 
    valueBack12345 = true;  
  } else {
    valueBack12345 = false;
  }
  return;
  //normaal gesproken is het een return IETS
}

void resetPin(){
  for (int p = 0; p < 4; p++) {
    pinCode[p] = 0;
  }
}
void printHex(byte *buffer, byte bufferSize) {/*Helper routine to dump a byte array as hex values to Serial.*/for (byte i = 0; i < bufferSize; i++) {Serial.print(buffer[i] < 0x10 ? " 0" : " ");Serial.print(buffer[i], HEX);}}
void printDec(byte *buffer, byte bufferSize) {/*Helper routine to dump a byte array as dec values to Serial.*/for (byte i = 0; i < bufferSize; i++) {Serial.print(buffer[i] < 0x10 ? " 0" : " ");Serial.print(buffer[i], DEC);}}
