 /*
 * set_time_and_date:
 *  Created on: 23 JAN 2022
 *   Author: NirK12
 */

#include "DS3231.h"

DS3231 rtc;
bool flag_date = false;
bool flag_time = false;
bool flag_done = false;
String s_date;
String s_time;



//SUN/30/1/22
bool dateExtraction(String d){
  String str= "";
  int i = 0;
  int num = 0;

  //find Day on Week
  while(d[i] != '\n' && d[i] != '\0' && d[i] != '/'){
    str += d[i];
    i += 1;
  }
  if(d[i] == '\n' || d[i] == '\0') return false;
  
  if(str == "SUN")rtc.DoW = 1;
  else if(str =="MON")rtc.DoW = 2;
  else if(str =="TUE")rtc.DoW = 3;
  else if(str =="WED")rtc.DoW = 4;
  else if(str =="THU")rtc.DoW = 5;
  else if(str =="FRI")rtc.DoW = 6;
  else if(str =="SAT")rtc.DoW = 7;
  else return false;

  //find Day on Month
  str = "";
  i += 1;
  while(d[i] != '\n' && d[i] != '\0' && d[i] != '/'){
    str += d[i];
    i += 1;
  }
  if(d[i] == '\n' || d[i] == '\0') return false;

  num = str.toInt();
  if(num >= 1 && num <= 31) rtc.DoM = num;
  else return false;

  //find Month
  str = "";
  i += 1;
  while(d[i] != '\n' && d[i] != '\0' && d[i] != '/'){
    str += d[i];
    i += 1;
  }
  if(d[i] == '\n' || d[i] == '\0') return false;

  num = str.toInt();
  if(num >= 1 && num <= 12) rtc.month = num;
  else return false;

  //find Year
  str = "";
  i += 1;
  while(d[i] != '\n' && d[i] != '\0'){
    str += d[i];
    i += 1;
  }
  if(d[i] == '/') return false;

  num = str.toInt();
  if(num >= 22 && num <= 99) rtc.year = num;
  else return false;

  return true;
}//END - dateExtraction



//13:45:00
bool timeExtraction(String t){
  String str= "";
  int i = 0;
  int num = 0;

  //find Hour
  while(t[i] != '\n' && t[i] != '\0' && t[i] != ':'){
    str += t[i];
    i += 1;
  }
  if(t[i] == '\n' || t[i] == '\0') return false;

  num = str.toInt();
  if(num >= 0 && num <= 23) rtc.hour = num;
  else return false;

  //find Minute
  str = "";
  i += 1;
  while(t[i] != '\n' && t[i] != '\0' && t[i] != ':'){
    str += t[i];
    i += 1;
  }
  if(t[i] == '\n' || t[i] == '\0') return false;

  num = str.toInt();
  if(num >= 0 && num <= 59) rtc.minute = num;
  else return false;

  //find Second
  str = "";
  i += 1;
  while(t[i] != '\n' && t[i] != '\0' && t[i] != ':'){
    str += t[i];
    i += 1;
  }
  if(t[i] == ':') return false;

  num = str.toInt();
  if(num >= 0 && num <= 59) rtc.second = num;
  else return false;

  return true;
}//END - timeExtraction




void setup(){
  Serial.begin(9600);
  Wire.begin();
  
  /******************* 
    SETUP TIME & DATE
  ******************/
  rtc.am_pm = false;  // Clock format: PM - false / AM - true 
  //rtc.DoW = 3;        //  Day in the Week - from 1 to 7 (1 = Sun, 2= Mon ...... , 7 = Saturday)
  //rtc.DoM = 25;       //  Day in the Month - from 1 up to (31,28,31,30,31,30,31,31,30,31,30,31)
  //rtc.month = 1;     //  Month number - from 1 to 12
  //rtc.year = 22;      //  Year number - give only the last 2 digits of a year (if 2022 then 22)
  //rtc.hour = 20;      //  Hour number - from 0 to 23  
  //rtc.minute = 6;    //  Minute number - from 0 to 59
  //rtc.second = 13;    //  Second number - from 0 to 59
  
  /******************* 
    CALL FUNCTION THAT SETUP - TIME & DATE
  ******************/
  //rtc.setDateAndTime();
  Serial.println("\n####################################################");
  Serial.println("################Welcome to RTC Setup################");
  Serial.println("####################################################");
  Serial.println("######Please read the instructions carefully!!!#####");
  Serial.println("####################################################");
  Serial.println("####################################################\nSUN - sunday\nMON - monday\nTUE - tuesday\nWED - wednesday\nTHU - thursday\nFRI - friday\nSAT - saturday\n################\n");
  Serial.println("Please enter the ~Date~ in specific format:\nSUN/30/1/22\n(Day on Week/Day on Month/Month/Year - ONLY 2 DIGITS!)");
}




void loop() {
  
  if(!flag_date && Serial.available() > 0){
    s_date = Serial.readString();
    if(dateExtraction(s_date)){
      flag_date = true;  
      Serial.println("###Date - Success!###");
      Serial.println("################Please enter the Time in specific format:################\n################ 13:0:50(Hour/Minute/Second)################");
    }
    else{
      Serial.println("###Error - Time format was not good!###\nPlease try again...\n");
      }
    }
    
  else if (!flag_time && Serial.available() > 0){
    s_time = Serial.readString(); 
    if(timeExtraction(s_time)){
      flag_time = true;
      Serial.println("###Time - Success!###");
    }
    else{
      Serial.println("###Error - Date format was not good!###\nPlease try again...\n");  
    } 
  }  
  if(!flag_done && flag_date && flag_time){
    flag_done = true;
    rtc.setDateAndTime();
    Serial.println("\n############################");
    Serial.println("####Date&Time was setup!####\n####Your RTC ready to use####\n#############Done!#############");   
  }
  else if(flag_done){
    Serial.println("\n"+rtc.getDate()+" # "+rtc.getTime());
    delay(1000); 
  }
  

}//END - loop
  
  

   
      
