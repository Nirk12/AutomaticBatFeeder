/*
 * main:
 *  Created on: 16 JAN 2022
 *   Author: NirK12
 */

#include <SD.h>
#include <SPI.h>
#include "sd_card.h"
#include "DS3231.h"
#include "data_message.h"
#include "ML134.h" 


//Global objects:
#define BUFFER_MESSAGE_SIZE  5  // "Future feature"
DS3231 rtc;
int bufferIndexWrite = 0;
int bufferIndexRead = 0;
DataMessage buffer_m[BUFFER_MESSAGE_SIZE]; // "Future feature"
ML134<HardwareSerial, RfidNotify> rfid(Serial); //RX pin serial
String FN; //feeder name


//Setup Arduino
void setup(){
  Serial.begin(9600);
  SD_CARD_init();      //init SD Card
  Wire.begin();        //init RTC legs - SCL+SDA
  FN = SD_CARD_get_feederName();
  
  if(SD_CARD_write_event_to_DBtxt(FN,rtc.getDate(),rtc.getTime(),"-","-",0,"SPO")){
    rfid.begin();
  }  
}



//Main loop
void loop(){  
  rfid.loop(FN,rtc);
  
}

//END
