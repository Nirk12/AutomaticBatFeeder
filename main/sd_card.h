 /*
 * sd_card.h
 *  Created on: 16 JAN 2022
 *   Author: Nir Kalay
 */

#ifndef _SD_CARD_H    
#define _SD_CARD_H    

//Tests Unit:
void testunit_get_feeder_name(void);

//END - Tests Unit





/*
Setup the sd card to Arduino Nano.
RETURN:
* 1 = Successful
* 0 = Not successful
*/
int SD_CARD_init(void);


/*
write to the sd card in specific format
GET - Strings to write.
RETURN:
* 1 = Successful
* 0 = Not successful
*/
int SD_CARD_toString_writeSD(String str_feeder,
                              String str_date,
                              String str_time,
                              String str_rfid,
                              String str_allowed,
                              String str_took,
                              String str_deserving,
                              String str_note);




/*
read from "NAME.txt" in sd card
GET - Void.
RETURN:
* String of "feeder name"
* e1 = empty file
* e2 = can't open this file - NAME.txt.
*/
String readSD_feederName(void);




/*
read from "BatList.txt" in sd card
GET - rfid.
RETURN:
* number of how much does a bat deserve to get food
*  0 = This bat does not deserve food.
* -3 = This bat not in our list.
* -4 = can't open this file - BatList.txt.
*/
int SD_CARD_get_bat_allowed(int rfid);




/*
GET - line of bat information.
RETURN:
* array of bat information.
*  [0] = rfid
*  [1] = This bat does deserve food? (0 = Not Deserve)/(1 = Deserve)
*  [2] = amount
*/
int * bat_information(String str);




#endif
