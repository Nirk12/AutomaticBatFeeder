/*
 * DS3231.h
 *  Created on: 25 JAN 2022
 *   Author: NirK12
 *
 *  DS3231 Real-Time Clock library for Arduino Nano
*/

#ifndef _DS3231_H
#define _DS3231_H

// Changed the following to work on 1.0
//#include "WProgram.h"
#include <Arduino.h>
#include <time.h>
#include <Wire.h>


//RTC
class DS3231 {
	
  private:
    // Convert normal decimal numbers to binary coded decimal
    byte decToBcd(byte val); 
    
    // Convert binary coded decimal to normal decimal numbers  
    byte bcdToDec(byte val); 

    /***************************************** 
    Functions - SET
    *****************************************/
    // In addition to setting the seconds, this clears the 
    // "Oscillator Stop Flag".
    void setSecond(byte Second); 

    // Sets the minute
    void setMinute(byte Minute); 
    
    // Sets the hour    
    void setHour(byte Hour); 
    
    // Sets the Day of the Week (1-7);  
    void setDoW(byte DoW); 

    // Sets the Date of the Month Day - (31,28,31,30,31,30,31,31,30,31,30,31)
    void setDoM(byte Date); 

    // Sets the Month of the year
    void setMonth(byte Month); 

    // Last two digits of the year  
    void setYear(byte Year); 

    // Set 12/24h mode. True is 12-h, false is 24-hour.
    // sets the mode to 12-hour (true) or 24-hour (false).
    // One thing that bothers me about how I've written this is that
    // if the read and right happen at the right hourly millisecnd,
    // the clock will be set back an hour. Not sure how to do it better, 
    // though, and as long as one doesn't set the mode frequently it's
    // a very minimal risk. 
    // It's zero risk if you call this BEFORE setting the hour, since
    // the setHour() function doesn't change this mode.
    void setClockMode(bool h12);

    /***************************************** 
    Functions - GET
    *****************************************/
    byte getSecond(); 
    byte getMinute();

    // In addition to returning the hour register, this function
    // returns the values of the 12/24-hour flag and the AM/PM flag.
    byte getHour(bool& h12, bool& PM_time); 

    //Day on Week 1-7
    byte getDoW(); 

    //Day on Month 1-31,1-30,1-28
    byte getDoM();
    
    // Also sets the flag indicating century roll-over. 
    byte getMonth(bool& Century); 

    // Last 2 digits only
    byte getYear(); 



    
  protected:
    // Read selected control byte: (0); reads 0x0e, (1) reads 0x0f
    byte readControlByte(bool which); 

    // Write the selected control byte. 
    // which == false -> 0x0e, true->0x0f.
    void writeControlByte(byte control, bool which); 




    
	public:	
    //Objects:
    bool am_pm = false;
    uint8_t DoW = 1; //Day on Week, 1 = Sun, 2= Mon ...... , 7 = Saturday.
		uint8_t DoM = 1; // the number in the date.
    uint8_t month = 1;
    uint8_t year = 20; //Only the last 2 digits,for 2022 enter only 22.  
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
		TwoWire & _Wire;


    /***************************************** 
    Class Constructors
    *****************************************/
		DS3231();
		DS3231(TwoWire & w);
    DS3231(bool aAM_PM,uint8_t aDoW,uint8_t aDoM, uint8_t aMonth, uint8_t aYear, uint8_t aHour, uint8_t aMinute, uint8_t aSecond);

    /***************************************** 
    Main Functions
    *****************************************/
    //setup date and time
    void setDateAndTime(void);
    
    // get Time in format "h:m:s"
    String getTime(void);
    
    //get Date in format "day number/m/y"
    String getDate(void);

    /***************************************** 
    Functions - TEMPERATURE
    *****************************************/
		
		// Temperature function
		float getTemperature(); 

    /***************************************** 
    Functions - ALARM
    *****************************************/
		
    /* Retrieves everything you could want to know about alarm
     * one. 
     * A1Dy true makes the alarm go on A1Day = Day of Week,
     * A1Dy false makes the alarm go on A1Day = Date of month.
     *
     * byte AlarmBits sets the behavior of the alarms:
     *  Dy  A1M4  A1M3  A1M2  A1M1  Rate
     *  X 1   1   1   1   Once per second
     *  X 1   1   1   0   Alarm when seconds match
     *  X 1   1   0   0   Alarm when min, sec match
     *  X 1   0   0   0   Alarm when hour, min, sec match
     *  0 0   0   0   0   Alarm when date, h, m, s match
     *  1 0   0   0   0   Alarm when DoW, h, m, s match
     *
     *  Dy  A2M4  A2M3  A2M2  Rate
     *  X 1   1   1   Once per minute (at seconds = 00)
     *  X 1   1   0   Alarm when minutes match
     *  X 1   0   0   Alarm when hours and minutes match
     *  0 0   0   0   Alarm when date, hour, min match
     *  1 0   0   0   Alarm when DoW, hour, min match
     */
		void getA1Time(byte& A1Day, byte& A1Hour, byte& A1Minute, byte& A1Second, byte& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM); 

    // Same as getA1Time();, but A2 only goes on seconds == 00.
		void getA2Time(byte& A2Day, byte& A2Hour, byte& A2Minute, byte& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM); 

    // Set the details for Alarm 1
		void setA1Time(byte A1Day, byte A1Hour, byte A1Minute, byte A1Second, byte AlarmBits, bool A1Dy, bool A1h12, bool A1PM); 

    // Set the details for Alarm 2  
		void setA2Time(byte A2Day, byte A2Hour, byte A2Minute, byte AlarmBits, bool A2Dy, bool A2h12, bool A2PM); 

    // Enables alarm 1 or 2 and the external interrupt pin.
    // If Alarm != 1, it assumes Alarm == 2.
		void turnOnAlarm(byte Alarm); 

    // Disables alarm 1 or 2 (default is 2 if Alarm != 1);
    // and leaves the interrupt pin alone.
		void turnOffAlarm(byte Alarm); 

    // Returns T/F to indicate whether the requested alarm is
    // enabled. Defaults to 2 if Alarm != 1.
		bool checkAlarmEnabled(byte Alarm); 

    // Checks whether the indicated alarm (1 or 2, 2 default);
    // has been activated.
		bool checkIfAlarm(byte Alarm); 



    
    /***************************************** 
    Functions - OSCILLATOR
    *****************************************/

    // turns oscillator on or off. True is on, false is off.
    // if battery is true, turns on even for battery-only operation,
    // otherwise turns off if Vcc is off.
    // frequency must be 0, 1, 2, or 3.
    // 0 = 1 Hz
    // 1 = 1.024 kHz
    // 2 = 4.096 kHz
    // 3 = 8.192 kHz (Default if frequency byte is out of range);
		void enableOscillator(bool TF, bool battery, byte frequency); 

    // Turns the 32kHz output pin on (true); or off (false).
		void enable32kHz(bool TF); 

    // Checks the status of the OSF (Oscillator Stop Flag);.
    // If this returns false, then the clock is probably not
    // giving you the correct time.
    // The OSF is cleared by function setSecond();  
		bool oscillatorCheck();

}; //EndOfClass

#endif
