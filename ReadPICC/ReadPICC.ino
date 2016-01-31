

/**
* ----------------------------------------------------------------------------
* This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
* for further details and other examples.
* 
* NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
* 
* Released into the public domain.
* ----------------------------------------------------------------------------
* This sample shows how to read and write data blocks on a MIFARE Classic PICC
* (= card/tag).
* 
* BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
* 
* 
* (pins on MFRC from left)
* Typical pin layout used:
* -----------------------------------------------------------------------------------------
*             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
*             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
* Signal      Pin          Pin           Pin       Pin        Pin              Pin
* -----------------------------------------------------------------------------------------
* RST/Reset   RST     7     9             5         D9         RESET/ICSP-5     RST
* SPI SS      SDA(SS) 1     10            53        D10        10               10
* SPI MOSI    MOSI    3     11 / ICSP-4   51        D11        ICSP-4           16
* SPI MISO    MISO    4     12 / ICSP-1   50        D12        ICSP-1           14
* SPI SCK     SCK     2     13 / ICSP-3   52        D13        ICSP-3           15
* 
* GND                 6
* 3.3V                8
*/

#include <SPI.h>
#include <MFRC522.h>
#include "Wire.h"
#include <Deuligne.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

// initialize the library with the numbers of the interface pins
Deuligne lcd;

/**
* Initialize.
*/
void setup() {
   Serial.begin(9600); // Initialize serial communications with the PC
   while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
   SPI.begin();        // Init SPI bus
   mfrc522.PCD_Init(); // Init MFRC522 card

   // Prepare the key (used both as key A and as key B)
   // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
   for (byte i = 0; i < 6; i++) {
       key.keyByte[i] = 0xFF;
   }

   // set antenna gain to max
   // mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

   // set up the LCD's number of columns and rows: 
   lcd.init();

   lcd.print(F("Scan ?"));
}

/**
* Main loop.
*/
void loop() {
   // Look for new cards
   if ( ! mfrc522.PICC_IsNewCardPresent()) {
     return;
   }

   lcd.clear();

   // Select one of the cards
   if ( ! mfrc522.PICC_ReadCardSerial())
       return;

   // Show some details of the PICC (that is: the tag/card)
   // set the cursor to column 0, line 0
   // (note: line 1 is the second row, since counting begins with 0):
   lcd.clear();
   lcd.setCursor(0, 0);

   lcd.print(F("U:"));
   dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
   lcd.setCursor(0, 1);

   // lcd.print(F("T:"));
   byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
   // lcd.print(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        lcd.setCursor(0, 0);
        lcd.print(F("Not MIFARE Classic"));
        return;
    }


    // In this code we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 1;
    byte blockAddr      = 4;

    union Block {
      long data;
      byte bytes[18];
    };
    byte trailerBlock   = 7;
    byte status;
    union Block buffer;
    byte size = sizeof(buffer);
    
    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Read data from the block
    status = mfrc522.MIFARE_Read(blockAddr, buffer.bytes, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data ")); Serial.println(buffer.data); 

    lcd.setCursor(0, 1);

    lcd.print(F("V: "));
    lcd.print(buffer.data);

    // Increment counter
    buffer.data += 1L;
    
    // Only for initialisation
    /* buffer.data = 0L;

    for (byte i = 0; i < 18; i++) {
        buffer.bytes[i] = 0x00;
    }
    */
    
    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Write data to the block
    status = mfrc522.MIFARE_Write(blockAddr, buffer.bytes, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    } else {
        lcd.print(F(" incr"));
    }
    Serial.println();
    


   // Halt PICC
   mfrc522.PICC_HaltA();
   // Stop encryption on PCD
   mfrc522.PCD_StopCrypto1();
}

/**
* Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
   for (byte i = 0; i < bufferSize; i++) {
       lcd.print(buffer[i] < 0x10 ? " 0" : " ");
       lcd.print(buffer[i], HEX);
   }
}
