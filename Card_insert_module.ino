/**********************************************************************************

   This was developed for the TMC26xx RLA board that is placed in the Quaco robot.
   The only pupose of this code is to initializes the tmc26x driver with required
   configuration values. Step and direction pulses will be sent from external
   microcontroller.

   IMPORTANT: This code should not be uploaded to microcontrollers with bootloader.
   microcontrollers with optiboot bootloader are slow to startup.
   hex file can be found at C:\Users\Sastra\AppData\Local\Temp\arduino_build_xxxxxx

   download TMC26X_config.exe tool for easily finding configuration settings
 **********************************************************************************/

#include <SPI.h>
#include <EEPROM.h>

/************************************ RMOT **********************************************/

#define CS_PIN_RMOT  10         //10 -RMOT  PMOT- 9 PB1

#define STEP_PIN_RMOT  A3      //A3 -RMOT  PMOT- 5 PD5
#define DIR_PIN_RMOT   A4       //A4 - RMOT PMOT- 6 PD6
#define EN_PIN_RMOT   A5        //A5 - RMOT  PMOT-7 PD7

/************************************ PMOT **********************************************/

#define CS_PIN_PMOT  9

#define STEP_PIN_PMOT  5      //A3 -RMOT  PMOT- 5 PD5
#define DIR_PIN_PMOT   6       //A4 - RMOT PMOT- 6 PD6
#define EN_PIN_PMOT   7        //A5 - RMOT  PMOT-7 PD7

/**********************************************************************************/
#define LIMIT_SW_PIN_RMOT   A1

struct DataStruct 
{
  int number;
};
int a=0,b=0;
int svalue, ivalue, saved_sValue; //swipe value is saved at EEPROM address 10
boolean dir = 0;
char cmd;
String command;
unsigned int i;

void writeIntIntoEEPROM(int address, int number)
{ 
  DataStruct data = {number};
  EEPROM.put(address, data);
}

int readIntFromEEPROM(int address)
{
  DataStruct data;
  EEPROM.get(address, data);
  return data.number;
}

void remove_card();
void insert_card();
void homing();
void tmc26x_spi_transfer(uint32_t txData, uint8_t cs_pin)
{
  uint8_t rxData;

  digitalWrite(CS_PIN_RMOT, HIGH);
  //select the TMC driver
  digitalWrite(cs_pin, LOW);

 // Serial.print("Sending ");
 // Serial.println(txData, HEX);

  //write/read the values
  rxData = SPI.transfer((txData >> 16) & 0xff);
  rxData <<= 8;
  rxData |= SPI.transfer((txData >>  8) & 0xff);
  rxData <<= 8;
  rxData |= SPI.transfer((txData) & 0xff);
  rxData >>= 4;

 // Serial.print("Received ");
 // Serial.println(rxData, HEX);

  //deselect the TMC chip
  digitalWrite(cs_pin, HIGH);
  //  digitalWrite(EN_PIN_RMOT, LOW);
}

void insert_card()
{
  EEPROM.get(10,saved_sValue);
 // insert_card_intoSlot();
  digitalWrite(DIR_PIN_RMOT, LOW);
  for (i = 0; i < (saved_sValue*80); i++)
  {
    digitalWrite(STEP_PIN_RMOT, LOW);
    delayMicroseconds(50);
    digitalWrite(STEP_PIN_RMOT, HIGH);
    delayMicroseconds(50);
  }
  //Serial.println("OK");
  delay(200);
}
void remove_card()
{
  EEPROM.get(10,saved_sValue);
 // insert_card_intoSlot();
   while (digitalRead(LIMIT_SW_PIN_RMOT) == HIGH)
  {
  digitalWrite(DIR_PIN_RMOT, HIGH);
  for (i = 0; i < (saved_sValue*80); i++)
  {
    digitalWrite(STEP_PIN_RMOT, LOW);
    delayMicroseconds(50);
    digitalWrite(STEP_PIN_RMOT, HIGH);
    delayMicroseconds(50);
  }
  //Serial.println("OK");
  delay(200);
}}

void homing()
{  
  while (digitalRead(LIMIT_SW_PIN_RMOT) == HIGH)
  {
    digitalWrite(DIR_PIN_RMOT, HIGH);
    digitalWrite(STEP_PIN_RMOT, LOW);
    delayMicroseconds(90);
    digitalWrite(STEP_PIN_RMOT, HIGH);
    delayMicroseconds(90);
  }

}

void setup()
{
  //delay for tmc26x setup/powerup
  delay(100);
  Serial.begin(115200);
  pinMode(LIMIT_SW_PIN_RMOT, INPUT);

  pinMode(CS_PIN_RMOT, OUTPUT);
  pinMode(CS_PIN_PMOT, OUTPUT);

  pinMode(STEP_PIN_RMOT, OUTPUT);
  pinMode(DIR_PIN_RMOT, OUTPUT);
  pinMode(EN_PIN_RMOT, OUTPUT);

  pinMode(STEP_PIN_PMOT, OUTPUT);
  pinMode(DIR_PIN_PMOT, OUTPUT);
  pinMode(EN_PIN_PMOT, OUTPUT);


  // configure the SPI interface
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setDataMode(SPI_MODE3);
  SPI.begin();
  /************************************ RMOT **********************************************/
  digitalWrite(CS_PIN_PMOT, HIGH);
  tmc26x_spi_transfer(0x000204, CS_PIN_RMOT);               //Stylus: 0x000204 //Pinch: 0x000204 //Rotate: 0x000204
  delay(5);
  tmc26x_spi_transfer(0x088AC1, CS_PIN_RMOT);               //Stylus: 0x080AC1 //Pinch:0x088AC1 //Rotate:0x088AC1
  delay(5);
  tmc26x_spi_transfer(0x0A0000, CS_PIN_RMOT);               //Stylus: 0x0A4001 //Pinch:0x0A0000 //Rotate:0x0A0000
  delay(5);
  tmc26x_spi_transfer(0x0D0505, CS_PIN_RMOT);              //Stylus: 0x0D0203 //Pinch:0x0D0505  //Rotate:0x0D0209
  delay(5);
  tmc26x_spi_transfer(0x0E0010, CS_PIN_RMOT);              //Stylus: 0x0E0010 //Pinch:0x0E0010  //Rotate:0x0E0010
  delay(5);


  digitalWrite(EN_PIN_RMOT, LOW);
  digitalWrite(DIR_PIN_RMOT, LOW);

  /************************************ PMOT **********************************************/
  digitalWrite(CS_PIN_RMOT, HIGH);
  tmc26x_spi_transfer(0x000204, CS_PIN_PMOT);               //Stylus: 0x000204 //Pinch: 0x000204 //Rotate: 0x000204
  delay(5);
  tmc26x_spi_transfer(0x088AC1, CS_PIN_PMOT);               //Stylus: 0x080AC1 //Pinch:0x088AC1 //Rotate:0x088AC1
  delay(5);
  tmc26x_spi_transfer(0x0A0000, CS_PIN_PMOT);               //Stylus: 0x0A4001 //Pinch:0x0A0000 //Rotate:0x0A0000
  delay(5);
  tmc26x_spi_transfer(0x0D0504, CS_PIN_PMOT);              //Stylus: 0x0D0203 //Pinch:0x0D0505  //Rotate:0x0D0209
  delay(5);
  tmc26x_spi_transfer(0x0E0010, CS_PIN_PMOT);              //Stylus: 0x0E0010 //Pinch:0x0E0010  //Rotate:0x0E0010
  delay(5);
  digitalWrite(CS_PIN_PMOT, HIGH);

  digitalWrite(EN_PIN_PMOT, LOW);
  digitalWrite(DIR_PIN_PMOT, LOW);

  delay(2000);

  homing();
}


void loop() {
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');

    if (command.startsWith("e:s:l:i:")) {
      if (command.length() < 10) {
        // Error 14: Argument length error
        Serial.println("14\n");
      } else {
        int svalue = command.substring(8).toInt();
        if (svalue < 0 || svalue > 500) {
          // Error 16: Parameter out of bounds
          Serial.println("16\n");
        } else {
          writeIntIntoEEPROM(10, svalue);
          saved_sValue = readIntFromEEPROM(10);
          //Serial.print("Swipe Value= ");
          Serial.println(saved_sValue);
          Serial.println("0\n");  // Indicate success with "0"
        }
      }
    } 

    else if (command == "r:") {
      //Serial.print("removing\n");
      //delay(100);
      remove_card();
      Serial.println("0\n");  // Indicate success with "0"
    } 
    else if (command == "i:") {
      //Serial.print("inserting\n");
      //delay(100);
      insert_card();
      Serial.println("0\n");  // Indicate success with "0"
    } 
    else if (command == "h:") {
      //Serial.print("homing\n");
      //delay(100);
      homing();
      Serial.println("0\n");  // Indicate success with "0"
    } 
    else if (command == "e:g:l:i:") {
      // Display the value stored in EEPROM from where "e:s:l:i:" value is saved
      saved_sValue = readIntFromEEPROM(10);
      //Serial.print("Saved Swipe Value= ");
      Serial.println(saved_sValue);
      Serial.println("0\n");  // Indicate success with "0"
    }
    else {
      // Error 13: Unrecognized command
      Serial.println("13\n");
    }
  }
}
