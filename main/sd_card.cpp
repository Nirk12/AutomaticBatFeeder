/*
 * sd_card.cpp
 *  Created on: 16 JAN 2022
 *   Author: Nir Kalay
 */

#include <SD.h>
#include <SPI.h>
#include "sd_card.h"

//Global objects:
File myFile;

const String FILE_RESULTS_REPORT = "DB.txt"; //Results report
const String FILE_FEEDER_NAME = "NAME.txt";
const String FILE_BAT_LIST = "BatList.txt";

const int PIN_CS = 2; // PIN D10 - on "Arduino Nano"
const int DELAY_BETWEEN_STEP = 200; 
const int RATE = 9600;
//const int RFID_LENGHT = 4; //1001
const String START = "Start";
const String HASH_MARK = "###";

static String FEEDER_NAME = "8888";//Defult name 
static int *bufferBatData; // [0]RFID###[1]Took###[2]Allowed


//Tests Unit:
void testunit_get_feeder_name(void){
    int res;
  res = SD_CARD_init();
  if(res == 0){
    Serial.println("NirPost>> Error in - SD_CARD_init()");
  }
  else{
    Serial.println("NirPost>> Seccess - SD_CARD_init()");
  }
  
  Serial.println("$$$");
  Serial.println(FEEDER_NAME);
  Serial.println("$$$");
}

//END - Tests Unit




int SD_CARD_init(void)
{
  //First Setup - SD Card configuration
  Serial.begin(RATE);
  //while (!Serial) {
     // wait for serial port to connect. Needed for native USB port only
  //}
  
  pinMode(PIN_CS,OUTPUT);

  // SD Card Initialzation
  if(SD.begin() ){
    Serial.println("NirPost>> SD card is ready to use.");
	
	  if(!(SD.exists(FILE_RESULTS_REPORT)) ){
		  myFile = SD.open(FILE_RESULTS_REPORT, FILE_WRITE);
		  delay(DELAY_BETWEEN_STEP);
		  myFile.println(START);
		  myFile.close();
	  }
  FEEDER_NAME = readSD_feederName();
	return 1;
  }
  
  else{
    Serial.println("NirPost>> SD card initialzation failed.");
	return 0;
  }
}// endOf func - SD_CARD_init




int SD_CARD_toString_writeSD(String str_feeder,
                              String str_date,
                              String str_time,
                              String str_rfid,
                              String str_allowed,
                              String str_took,
                              String str_deserving,
                              String str_note){
  
  String finalStr = str_feeder + HASH_MARK
                   + str_date + HASH_MARK
                   + str_time + HASH_MARK
                   + str_rfid + HASH_MARK
                   + str_allowed + HASH_MARK 
                   + str_took + HASH_MARK
                   + str_deserving + HASH_MARK
                   + str_note;
                        
  //Start - Create/Open file
  myFile = SD.open(FILE_RESULTS_REPORT, FILE_WRITE);

  // if the file opened okay, write to it:
  if(myFile){
    Serial.println("NirPost>> Writing to file new bat update - DB.txt");
    //
    //Write to file-->
	
    delay(DELAY_BETWEEN_STEP);
	  myFile.println(finalStr);
    myFile.close();
    Serial.println("Done.");
    return 1; // Successful = 1
  }
  
  // if the file didn't open, print an error:
  else{
    Serial.println("NirPost>> Error opening "+ FILE_RESULTS_REPORT);
	  return 0;
  }

}//endOf func - SD_CARD_write




String readSD_feederName(void){
  File file = SD.open(FILE_FEEDER_NAME, FILE_READ); //FILE_RESULTS_REPORT - FILE_FEEDER_NAME - FILE_BAT_LIST
  //delay(1000);
  if(file){
    Serial.println("NirPost>> Reading from file - " + FILE_FEEDER_NAME);
    String res;
    boolean findFormat = false;
    
    while(file.available() ){
      res += char(file.read());
      if(res == "feederName###"){
        res = "";
        findFormat = true;    
      }
    }
    file.close();

    if(!findFormat)
      return "e1";  
    return res;
  }
  else{
    Serial.println("NirPost>> Error opening "+ FILE_FEEDER_NAME);
    return "e2";
  }

}//endOf func - readSD_feederName





int SD_CARD_get_bat_allowed(int rfid){
  File file = SD.open(FILE_BAT_LIST, FILE_READ); //FILE_RESULTS_REPORT - FILE_FEEDER_NAME - FILE_BAT_LIST
  //delay(rfid);
  if(file){
    Serial.println("NirPost>> Reading from file - " + FILE_BAT_LIST);
    
    String res;
    char temp_c;
    boolean findBat = false;
 
    while(file.available() ){
      temp_c = char(file.read());

      //finish to read number
      if(!findBat && temp_c == '#'){
        
       //the number is the bat we looking for?
        if(rfid == res.toInt() ){
          findBat = true;
        }
        else{
          //jump to the next bat
          while(file.available() && temp_c != '\n'){
            temp_c = char(file.read());
          }
          res="";
        }
      }
      if(!findBat && temp_c != '\n'){
        res+=temp_c; // 1001
      }
      else if(findBat){
        while(temp_c != '\n'){
          res+=temp_c; // 1001
          temp_c = char(file.read());
        }
        break;
      }
    }
    
    file.close();
    
    if(!findBat){
      return -3;  
    }
    
    bufferBatData = bat_information(res);
    //Serial.println("[0]=" + String(bufferBatData[0]) + ", [1]=" + String(bufferBatData[1])+ ", [2]=" + String(bufferBatData[2])   );

    if(bufferBatData[1])
      return bufferBatData[2]; //amount piece of food.
    //This bat does not deserve food.
    else{
      return 0;
    }
  }
  
  //if this file can't open
  else{
    Serial.println("NirPost>> Error opening "+ FILE_BAT_LIST);
    return -4;
  }
}//endOf func - SD_CARD_get_bat_allowed




int * bat_information(String str){
  static int batInfo[3]={-1};
  int loc =0;
  String number;

  for(int i=0;i<3;i++){
    while( !(str[loc] == '#' && str[loc+1] == '#' && str[loc+2] == '#') ){
      number+= str[loc];
      loc+= 1;
    }
    number[loc]='\n';
    batInfo[i] = number.toInt();
    number="";
    loc+=3;
  }
    

  //Serial.println(batInfo[0]);
  
  return batInfo;
}//endOf func - [] bat_information()
