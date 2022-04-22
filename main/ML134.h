/*
 * ML134.h
 *  Created on: 08 FAB 2022
 *   Author: NirK12
 *
 *  
*/
#ifndef _ML134_H
#define _ML134_H

#include "sd_card.h"
#include "DS3231.h"


//RFID_Error_None = 0
//RFID_Error_PacketSize             = 0x81 = 129
//RFID_Error_PacketEndCodeMissmatch = 0x82 = 130
//RFID_Error_PacketChecksum         = 0x83 = 131
//RFID_Error_PacketChecksumInvert   = 0x84 = 132
enum RFID_Error{
    RFID_Error_None,

    // from library
    RFID_Error_PacketSize = 0x81,
    RFID_Error_PacketEndCodeMissmatch,
    RFID_Error_PacketChecksum,
    RFID_Error_PacketChecksumInvert
};




//RFID data - The structure of the information we are likely to receive and use
typedef struct Reading{
    uint16_t country;
    uint64_t id;
    bool isData;
    bool isAnimal;
    uint16_t reserved0;
    uint32_t reserved1;
}RFIDReading;





template<class T_SERIAL_METHOD, class T_NOTIFICATION_METHOD  > class ML134{
	public:
	
	
    ML134(T_SERIAL_METHOD& serial) : _serial(serial){}


    void begin(void){
      _serial.setTimeout(1000);
    }


    void loop(String FN, DS3231 rtc){
      int flag;
      
      // RFID_Packet_SIZE == 30
      while (_serial.available() >= RFID_Packet_SIZE){ 
        String res = readPacket();               // RFID_Error_None == 0
        flag = SD_CARD_write_event_to_DBtxt(FN,rtc.getDate(),rtc.getTime(),res,"Yes",0,"n");
        if(flag == 1){
          Serial.println(res);
        }
        else{
         Serial.println("Error"); 
        }
      }
  }


	private:
    // "_serial" will be the object we will work with throughout the library
		T_SERIAL_METHOD& _serial;

		enum DfMp3_Packet
		{
			RFID_Packet_StartCode,
			RFID_Packet_Id = 1,
			RFID_Packet_Country = 11,
			RFID_Packet_DataFlag = 15,
			RFID_Packet_AnimalFlag = 16,
			RFID_Packet_Reserved0 = 17,
			RFID_Packet_Reserved1 = 21,
			RFID_Packet_CheckSum = 27,
			RFID_Packet_CheckSumInvert = 28,
			RFID_Packet_EndCode = 29,
			RFID_Packet_SIZE 
		};



  String readPacket(void){
          
      char packet[RFID_Packet_SIZE];
      packet[RFID_Packet_StartCode] = _serial.read();

      // check for the first byte being the packet start code -  0x02 is good!
      if (packet[RFID_Packet_StartCode] != 0x02){
          // just out of sync, ignore until we are synced up
          return "1";// "RFID_Error_None";
      }

      uint8_t read;

      read = _serial.readBytes(&(packet[RFID_Packet_Id]), RFID_Packet_SIZE - 1);

      //Look for errors:
      if (read != RFID_Packet_SIZE - 1){
          return "2";// "RFID_Error_PacketSize";
      } 

      if (packet[RFID_Packet_EndCode] != 0x03){
          return "3";// "RFID_Error_PacketEndCodeMissmatch";
      }

      // calculate 
      uint8_t checksum = 0;
      for (uint8_t i = RFID_Packet_Id; i < RFID_Packet_CheckSum; i++){
          checksum = checksum ^ packet[i];
      }

      // test checksum
      if (checksum != packet[RFID_Packet_CheckSum]){
          return "4";// "RFID_Error_PacketChecksum"
      }

      if (static_cast<uint8_t>(~checksum) != static_cast<uint8_t>(packet[RFID_Packet_CheckSumInvert])){
          return "5";// "RFID_Error_PacketChecksumInvert"
     }

      
      RFIDReading reading;

      // The tests were successful and therefore:
      // convert packet into the reading struct
      reading.id = HexLsbAsciiToUint64(&(packet[RFID_Packet_Id]), RFID_Packet_Country - RFID_Packet_Id);
      reading.country = HexLsbAsciiToUint64(&(packet[RFID_Packet_Country]), RFID_Packet_DataFlag - RFID_Packet_Country);
      reading.isData = packet[RFID_Packet_DataFlag] == '1';
      reading.isAnimal = packet[RFID_Packet_AnimalFlag] == '1';
      reading.reserved0 = HexLsbAsciiToUint64(&(packet[RFID_Packet_Reserved0]), RFID_Packet_Reserved1 - RFID_Packet_Reserved0);
      reading.reserved1 = HexLsbAsciiToUint64(&(packet[RFID_Packet_Reserved1]), RFID_Packet_CheckSum - RFID_Packet_Reserved1);

	    String res;
      res = T_NOTIFICATION_METHOD::OnPacketRead(reading);
  
      return res;
  }


  uint64_t HexLsbAsciiToUint64(char* text, uint8_t textSize){
      uint64_t value = 0;
      uint8_t i = textSize;
      do 
      {
          i--;

          uint8_t digit = text[i];
          if (digit >= 'A'){
              digit = digit - 'A' + 10;
          }
          else{
              digit = digit - '0';
          }
          value = (value << 4) + digit;
      } while (i != 0);

      return value;
  }
  
};





class RfidNotify
{
	public:
	
//	  // ML134_Errors for code meaning
//    static void OnError(RFID_Error errorCode){  
//      Serial.print("\nCom Error: " + errorCode);
//    }


    static String OnPacketRead(const RFIDReading& reading){
      char temp[8];
      String rfid;

      // since print doesn't support leading zero's, use sprintf
      //sprintf(temp, "%03u", reading.country);
      ////rfid += temp;
        
    
      // break it up into equal sized leading zero pieces
      sprintf(temp, "%06lu", static_cast<uint32_t>(reading.id / 1000000));
      rfid += temp;
      sprintf(temp, "%06lu", static_cast<uint32_t>(reading.id % 1000000));
      rfid += temp;
      //Serial.println(rfid);
//      Serial.print(" ");
//      if (reading.isData){
//        Serial.print("Have data!");
//      }
//      if (reading.isAnimal){
//        Serial.print("The data is an animal");
//      }
//      Serial.println();
    return rfid;
	}
    
    
};


#endif
