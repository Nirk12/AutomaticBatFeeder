/*
 * main:
 *  Created on: 16 JAN 2022
 *   Author: NirK12
 */

#include <SD.h>
#include <SPI.h>
#include "bat.h"
#include "sd_card.h"
#include "DS3231.h"

//Global objects:
DS3231 rtc;



//SD+DS3231
void regression(void){
   String d = rtc.getDate();
   String t = rtc.getTime();
   Serial.println("\n\n###Start Regression ### ("+ d+" # "+t+ ")");
//   
//   Serial.println("1) Check Feeder name from NAME.txt");
//   if(SD_CARD_get_feederName() == "101"){
//    Serial.println("1) - PASS");
//   }
//   else{
//    Serial.println("1) - FAIL");
//   }
//   /////////////
//   Serial.println("2) Check if specific bat in the BatList.txt:");
//   if(SD_CARD_get_bat_allowed(1002) > 0 ){
//    Serial.println("2) - PASS");
//   }
//   else{
//    Serial.println("2) - FAIL");
//   }
   /////////////
   Serial.println("3) Check if we can write specific bat to SD Card with Date&Time from RTC");
   if(SD_CARD_write_event_to_DBtxt(SD_CARD_get_feederName(),
                              d,
                              t,
                              1212,
                              "yes",
                              1,
                              "none") == 1 ){
    Serial.println("3) - PASS");
   }
   else{
    Serial.println("3) - FAIL");
   }


    //Serial.println("\n\n###End Regression ### ("+ rtc.getDate()+" # "+rtc.getTime()+ ")");
}



void setup(){
  Serial.begin(9600);
  SD_CARD_init();      //init SD Card
  Wire.begin();        //init RTC legs - SCL+SDA


  regression();   
}
void loop(){
  //Serial.println("\n"+rtc.getDate()+" # "+rtc.getTime());
  //delay(1000);
}

//END
