/*
 * DS3231.cpp
 *  Created on: 25 JAN 2022
 *   Author: NirK12
 *
 *  DS3231 Real-Time Clock library for Arduino Nano
*/
#include "DS3231.h"

#define CLOCK_ADDRESS 0x68
#define SECONDS_FROM_1970_TO_2000 946684800


//Memory of day in the month
static const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };


/***************************************** 
  DS3231 - Constructors
 *****************************************/
DS3231::DS3231() : _Wire(Wire) {
	// nothing to do for this constructor.
}

DS3231::DS3231(TwoWire & w) : _Wire(w) {
  // nothing to do for this constructor.
}

DS3231::DS3231(bool aAM_PM,uint8_t aDoW,uint8_t aDoM, uint8_t aMonth, uint8_t aYear, uint8_t aHour, uint8_t aMinute, uint8_t aSecond){
  bool am_pm = aAM_PM;
  uint8_t DoW = aDoW;
  uint8_t DoM = aDoM;
  uint8_t month = aMonth;
  uint8_t year = aYear; 
  uint8_t hour = aHour;
  uint8_t minute = aMinute;
  uint8_t second = aSecond;
}


/***************************************** 
	DS3231 - Public Functions
 *****************************************/
 
/*******************************************************************************
 * To get all Date/Time information at once use:
 * 1) String getTime(void)
 * 2) String getDate(void)
 ******************************************************************************/

void DS3231::setDateAndTime(void){
  //AM or PM
  setClockMode(am_pm);
  //Clock
  setSecond(second); 
  setMinute(minute); 
  setHour(hour); 
  //Date
  setDoW(DoW); // Sets the Day of the Week (1-7);
  setDoM(DoM); // Sets the Date of the Month
  setMonth(month); 
  setYear(year); // Last two digits of the year
}




//uint8_t yOff, m, d, hh, mm, ss;
// get Time in format "h:m:s"
String DS3231::getTime(void){
  
  String rtc_time = "";
  bool h_12_24;
  bool am_pm;
  rtc_time = String(getHour(h_12_24,am_pm) )+":"
              +String(getMinute() ) +":"
              +String(getSecond() );
  return rtc_time;
}




//get Date in format "day number/m/y"
String DS3231::getDate(void){
  
  String rtc_date = "";
  bool cen;
  rtc_date = String(getDoM() )+"/"
              +String(getMonth(cen) )+"/"
              +String(getYear() );
  return rtc_date;
}




byte DS3231::getSecond() {
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x00);
	_Wire.endTransmission();

	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	return bcdToDec(_Wire.read());
}




byte DS3231::getMinute() {
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x01);
	_Wire.endTransmission();

	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	return bcdToDec(_Wire.read());
}




byte DS3231::getHour(bool& h12, bool& PM_time) {
	byte temp_buffer;
	byte hour;
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x02);
	_Wire.endTransmission();

	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	temp_buffer = _Wire.read();
	h12 = temp_buffer & 0b01000000;
	if (h12) {
		PM_time = temp_buffer & 0b00100000;
		hour = bcdToDec(temp_buffer & 0b00011111);
	} else {
		hour = bcdToDec(temp_buffer & 0b00111111);
	}
	return hour;
}




byte DS3231::getDoW() {
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x03);
	_Wire.endTransmission();

	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	return bcdToDec(_Wire.read());
}




byte DS3231::getDoM() {
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x04);
	_Wire.endTransmission();

	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	return bcdToDec(_Wire.read());
}




byte DS3231::getMonth(bool& Century) {
	byte temp_buffer;
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x05);
	_Wire.endTransmission();

	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	temp_buffer = _Wire.read();
	Century = temp_buffer & 0b10000000;
	return (bcdToDec(temp_buffer & 0b01111111)) ;
}




byte DS3231::getYear() {
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x06);
	_Wire.endTransmission();

	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	return bcdToDec(_Wire.read());
}






void DS3231::setSecond(byte Second) {
	// Sets the seconds 
	// This function also resets the Oscillator Stop Flag, which is set
	// whenever power is interrupted.
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x00);
	_Wire.write(decToBcd(Second));	
	_Wire.endTransmission();
	// Clear OSF flag
	byte temp_buffer = readControlByte(1);
	writeControlByte((temp_buffer & 0b01111111), 1);
}




void DS3231::setMinute(byte Minute) {
	// Sets the minutes 
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x01);
	_Wire.write(decToBcd(Minute));	
	_Wire.endTransmission();
}




void DS3231::setHour(byte Hour) {
	// Sets the hour, without changing 12/24h mode.
	// The hour must be in 24h format.

	bool h12;
	byte temp_hour;

	// Start by figuring out what the 12/24 mode is
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x02);
	_Wire.endTransmission();
	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	h12 = (_Wire.read() & 0b01000000);
	// if h12 is true, it's 12h mode; false is 24h.

	if (h12) {
		// 12 hour
		bool am_pm = (Hour > 11);
		temp_hour = Hour;
		if (temp_hour > 11) {
			temp_hour = temp_hour - 12;
		}
		if (temp_hour == 0) {
			temp_hour = 12;
		}
		temp_hour = decToBcd(temp_hour) | (am_pm << 5) | 0b01000000;
	} else {
		// 24 hour
		temp_hour = decToBcd(Hour) & 0b10111111;
	}

	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x02);
	_Wire.write(temp_hour);
	_Wire.endTransmission();
}




void DS3231::setDoW(byte DoW) {
	// Sets the Day of Week
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x03);
	_Wire.write(decToBcd(DoW));	
	_Wire.endTransmission();
}




void DS3231::setDoM(byte Date) {
	// Sets the Date
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x04);
	_Wire.write(decToBcd(Date));	
	_Wire.endTransmission();
}




void DS3231::setMonth(byte Month) {
	// Sets the month
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x05);
	_Wire.write(decToBcd(Month));	
	_Wire.endTransmission();
}




void DS3231::setYear(byte Year) {
	// Sets the year
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x06);
	_Wire.write(decToBcd(Year));	
	_Wire.endTransmission();
}




void DS3231::setClockMode(bool h12) {
	
	byte temp_buffer;

	// Start by reading byte 0x02.
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x02);
	_Wire.endTransmission();
	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	temp_buffer = _Wire.read();

	// Set the flag to the requested value:
	if (h12) {
		temp_buffer = temp_buffer | 0b01000000;
	} else {
		temp_buffer = temp_buffer & 0b10111111;
	}

	// Write the byte
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x02);
	_Wire.write(temp_buffer);
	_Wire.endTransmission();
}




float DS3231::getTemperature() {
	// Checks the internal thermometer on the DS3231 and returns the 
	// temperature as a floating-point value.

  // Updated / modified a tiny bit from "Coding Badly" and "Tri-Again"
  // http://forum.arduino.cc/index.php/topic,22301.0.html
  
  byte tMSB, tLSB;
  float temp3231;
  
  // temp registers (11h-12h) get updated automatically every 64s
  _Wire.beginTransmission(CLOCK_ADDRESS);
  _Wire.write(0x11);
  _Wire.endTransmission();
  _Wire.requestFrom(CLOCK_ADDRESS, 2);

  // Should I do more "if available" checks here?
  if(_Wire.available()) {
    tMSB = _Wire.read(); //2's complement int portion
    tLSB = _Wire.read(); //fraction portion

    int16_t  itemp  = ( tMSB << 8 | (tLSB & 0xC0) );  // Shift upper byte, add lower
    temp3231 = ( (float)itemp / 256.0 );              // Scale and return
  }
  else {
    temp3231 = -9999; // Impossible temperature; error value
  }
   
  return temp3231;
}




void DS3231::getA1Time(byte& A1Day, byte& A1Hour, byte& A1Minute, byte& A1Second, byte& AlarmBits, bool& A1Dy, bool& A1h12, bool& A1PM) {
	byte temp_buffer;
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x07);
	_Wire.endTransmission();

	_Wire.requestFrom(CLOCK_ADDRESS, 4);

	temp_buffer	= _Wire.read();	// Get A1M1 and A1 Seconds
	A1Second	= bcdToDec(temp_buffer & 0b01111111);
	// put A1M1 bit in position 0 of DS3231_AlarmBits.
	AlarmBits	= AlarmBits | (temp_buffer & 0b10000000)>>7;

	temp_buffer		= _Wire.read();	// Get A1M2 and A1 minutes
	A1Minute	= bcdToDec(temp_buffer & 0b01111111);
	// put A1M2 bit in position 1 of DS3231_AlarmBits.
	AlarmBits	= AlarmBits | (temp_buffer & 0b10000000)>>6;

	temp_buffer	= _Wire.read();	// Get A1M3 and A1 Hour
	// put A1M3 bit in position 2 of DS3231_AlarmBits.
	AlarmBits	= AlarmBits | (temp_buffer & 0b10000000)>>5;
	// determine A1 12/24 mode
	A1h12		= temp_buffer & 0b01000000;
	if (A1h12) {
		A1PM	= temp_buffer & 0b00100000;			// determine am/pm
		A1Hour	= bcdToDec(temp_buffer & 0b00011111);	// 12-hour
	} else {
		A1Hour	= bcdToDec(temp_buffer & 0b00111111);	// 24-hour
	}

	temp_buffer	= _Wire.read();	// Get A1M4 and A1 Day/Date
	// put A1M3 bit in position 3 of DS3231_AlarmBits.
	AlarmBits	= AlarmBits | (temp_buffer & 0b10000000)>>4;
	// determine A1 day or date flag
	A1Dy		= (temp_buffer & 0b01000000)>>6;
	if (A1Dy) {
		// alarm is by day of week, not date.
		A1Day	= bcdToDec(temp_buffer & 0b00001111);
	} else {
		// alarm is by date, not day of week.
		A1Day	= bcdToDec(temp_buffer & 0b00111111);
	}
}




void DS3231::getA2Time(byte& A2Day, byte& A2Hour, byte& A2Minute, byte& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM) {
	byte temp_buffer;
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x0b);
	_Wire.endTransmission();

	_Wire.requestFrom(CLOCK_ADDRESS, 3); 
	temp_buffer	= _Wire.read();	// Get A2M2 and A2 Minutes
	A2Minute	= bcdToDec(temp_buffer & 0b01111111);
	// put A2M2 bit in position 4 of DS3231_AlarmBits.
	AlarmBits	= AlarmBits | (temp_buffer & 0b10000000)>>3;

	temp_buffer	= _Wire.read();	// Get A2M3 and A2 Hour
	// put A2M3 bit in position 5 of DS3231_AlarmBits.
	AlarmBits	= AlarmBits | (temp_buffer & 0b10000000)>>2;
	// determine A2 12/24 mode
	A2h12		= temp_buffer & 0b01000000;
	if (A2h12) {
		A2PM	= temp_buffer & 0b00100000;			// determine am/pm
		A2Hour	= bcdToDec(temp_buffer & 0b00011111);	// 12-hour
	} else {
		A2Hour	= bcdToDec(temp_buffer & 0b00111111);	// 24-hour
	}

	temp_buffer	= _Wire.read();	// Get A2M4 and A1 Day/Date
	// put A2M4 bit in position 6 of DS3231_AlarmBits.
	AlarmBits	= AlarmBits | (temp_buffer & 0b10000000)>>1;
	// determine A2 day or date flag
	A2Dy		= (temp_buffer & 0b01000000)>>6;
	if (A2Dy) {
		// alarm is by day of week, not date.
		A2Day	= bcdToDec(temp_buffer & 0b00001111);
	} else {
		// alarm is by date, not day of week.
		A2Day	= bcdToDec(temp_buffer & 0b00111111);
	}
}




void DS3231::setA1Time(byte A1Day, byte A1Hour, byte A1Minute, byte A1Second, byte AlarmBits, bool A1Dy, bool A1h12, bool A1PM) {
	//	Sets the alarm-1 date and time on the DS3231, using A1* information
	byte temp_buffer;
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x07);	// A1 starts at 07h
	// Send A1 second and A1M1
	_Wire.write(decToBcd(A1Second) | ((AlarmBits & 0b00000001) << 7));
	// Send A1 Minute and A1M2
	_Wire.write(decToBcd(A1Minute) | ((AlarmBits & 0b00000010) << 6));
	// Figure out A1 hour 
	if (A1h12) {
		// Start by converting existing time to h12 if it was given in 24h.
		if (A1Hour > 12) {
			// well, then, this obviously isn't a h12 time, is it?
			A1Hour = A1Hour - 12;
			A1PM = true;
		}
		if (A1PM) {
			// Afternoon
			// Convert the hour to BCD and add appropriate flags.
			temp_buffer = decToBcd(A1Hour) | 0b01100000;
		} else {
			// Morning
			// Convert the hour to BCD and add appropriate flags.
			temp_buffer = decToBcd(A1Hour) | 0b01000000;
		}
	} else {
		// Now for 24h
		temp_buffer = decToBcd(A1Hour); 
	}
	temp_buffer = temp_buffer | ((AlarmBits & 0b00000100)<<5);
	// A1 hour is figured out, send it
	_Wire.write(temp_buffer); 
	// Figure out A1 day/date and A1M4
	temp_buffer = ((AlarmBits & 0b00001000)<<4) | decToBcd(A1Day);
	if (A1Dy) {
		// Set A1 Day/Date flag (Otherwise it's zero)
		temp_buffer = temp_buffer | 0b01000000;
	}
	_Wire.write(temp_buffer);
	// All done!
	_Wire.endTransmission();
}





void DS3231::setA2Time(byte A2Day, byte A2Hour, byte A2Minute, byte AlarmBits, bool A2Dy, bool A2h12, bool A2PM) {
	//	Sets the alarm-2 date and time on the DS3231, using A2* information
	byte temp_buffer;
	_Wire.beginTransmission(CLOCK_ADDRESS);
	_Wire.write(0x0b);	// A1 starts at 0bh
	// Send A2 Minute and A2M2
	_Wire.write(decToBcd(A2Minute) | ((AlarmBits & 0b00010000) << 3));
	// Figure out A2 hour 
	if (A2h12) {
		// Start by converting existing time to h12 if it was given in 24h.
		if (A2Hour > 12) {
			// well, then, this obviously isn't a h12 time, is it?
			A2Hour = A2Hour - 12;
			A2PM = true;
		}
		if (A2PM) {
			// Afternoon
			// Convert the hour to BCD and add appropriate flags.
			temp_buffer = decToBcd(A2Hour) | 0b01100000;
		} else {
			// Morning
			// Convert the hour to BCD and add appropriate flags.
			temp_buffer = decToBcd(A2Hour) | 0b01000000;
		}
	} else {
		// Now for 24h
		temp_buffer = decToBcd(A2Hour); 
	}
	// add in A2M3 bit
	temp_buffer = temp_buffer | ((AlarmBits & 0b00100000)<<2);
	// A2 hour is figured out, send it
	_Wire.write(temp_buffer); 
	// Figure out A2 day/date and A2M4
	temp_buffer = ((AlarmBits & 0b01000000)<<1) | decToBcd(A2Day);
	if (A2Dy) {
		// Set A2 Day/Date flag (Otherwise it's zero)
		temp_buffer = temp_buffer | 0b01000000;
	}
	_Wire.write(temp_buffer);
	// All done!
	_Wire.endTransmission();
}




void DS3231::turnOnAlarm(byte Alarm) {
	// turns on alarm number "Alarm". Defaults to 2 if Alarm is not 1.
	byte temp_buffer = readControlByte(0);
	// modify control byte
	if (Alarm == 1) {
		temp_buffer = temp_buffer | 0b00000101;
	} else {
		temp_buffer = temp_buffer | 0b00000110;
	}
	writeControlByte(temp_buffer, 0);
}

void DS3231::turnOffAlarm(byte Alarm) {
	// turns off alarm number "Alarm". Defaults to 2 if Alarm is not 1.
	// Leaves interrupt pin alone.
	byte temp_buffer = readControlByte(0);
	// modify control byte
	if (Alarm == 1) {
		temp_buffer = temp_buffer & 0b11111110;
	} else {
		temp_buffer = temp_buffer & 0b11111101;
	}
	writeControlByte(temp_buffer, 0);
}




bool DS3231::checkAlarmEnabled(byte Alarm) {
	// Checks whether the given alarm is enabled.
	byte result = 0x0;
	byte temp_buffer = readControlByte(0);
	if (Alarm == 1) {
		result = temp_buffer & 0b00000001;
	} else {
		result = temp_buffer & 0b00000010;
	}
	return result;
}




bool DS3231::checkIfAlarm(byte Alarm) {
	// Checks whether alarm 1 or alarm 2 flag is on, returns T/F accordingly.
	// Turns flag off, also.
	// defaults to checking alarm 2, unless Alarm == 1.
	byte result;
	byte temp_buffer = readControlByte(1);
	if (Alarm == 1) {
		// Did alarm 1 go off?
		result = temp_buffer & 0b00000001;
		// clear flag
		temp_buffer = temp_buffer & 0b11111110;
	} else {
		// Did alarm 2 go off?
		result = temp_buffer & 0b00000010;
		// clear flag
		temp_buffer = temp_buffer & 0b11111101;
	}
	writeControlByte(temp_buffer, 1);
	return result;
}




void DS3231::enableOscillator(bool TF, bool battery, byte frequency) {
	// turns oscillator on or off. True is on, false is off.
	// if battery is true, turns on even for battery-only operation,
	// otherwise turns off if Vcc is off.
	// frequency must be 0, 1, 2, or 3.
	// 0 = 1 Hz
	// 1 = 1.024 kHz
	// 2 = 4.096 kHz
	// 3 = 8.192 kHz (Default if frequency byte is out of range)
	if (frequency > 3) frequency = 3;
	// read control byte in, but zero out current state of RS2 and RS1.
	byte temp_buffer = readControlByte(0) & 0b11100111;
	if (battery) {
		// turn on BBSQW flag
		temp_buffer = temp_buffer | 0b01000000;
	} else {
		// turn off BBSQW flag
		temp_buffer = temp_buffer & 0b10111111;
	}
	if (TF) {
		// set ~EOSC to 0 and INTCN to zero.
		temp_buffer = temp_buffer & 0b01111011;
	} else {
		// set ~EOSC to 1, leave INTCN as is.
		temp_buffer = temp_buffer | 0b10000000;
	}
	// shift frequency into bits 3 and 4 and set.
	frequency = frequency << 3;
	temp_buffer = temp_buffer | frequency;
	// And write the control bits
	writeControlByte(temp_buffer, 0);
}




void DS3231::enable32kHz(bool TF) {
	// turn 32kHz pin on or off
	byte temp_buffer = readControlByte(1);
	if (TF) {
		// turn on 32kHz pin
		temp_buffer = temp_buffer | 0b00001000;
	} else {
		// turn off 32kHz pin
		temp_buffer = temp_buffer & 0b11110111;
	}
	writeControlByte(temp_buffer, 1);
}




bool DS3231::oscillatorCheck() {
	// Returns false if the oscillator has been off for some reason.
	// If this is the case, the time is probably not correct.
	byte temp_buffer = readControlByte(1);
	bool result = true;
	if (temp_buffer & 0b10000000) {
		// Oscillator Stop Flag (OSF) is set, so return false.
		result = false;
	}
	return result;
}

/***************************************** 
	Private Functions
 *****************************************/

byte DS3231::decToBcd(byte val) {
// Convert normal decimal numbers to binary coded decimal
	return ( (val/10*16) + (val%10) );
}




byte DS3231::bcdToDec(byte val) {
// Convert binary coded decimal to normal decimal numbers
	return ( (val/16*10) + (val%16) );
}




byte DS3231::readControlByte(bool which) {
	// Read selected control byte
	// first byte (0) is 0x0e, second (1) is 0x0f
	_Wire.beginTransmission(CLOCK_ADDRESS);
	if (which) {
		// second control byte
		_Wire.write(0x0f);
	} else {
		// first control byte
		_Wire.write(0x0e);
	}
	_Wire.endTransmission();
	_Wire.requestFrom(CLOCK_ADDRESS, 1);
	return _Wire.read();	
}




void DS3231::writeControlByte(byte control, bool which) {
	// Write the selected control byte.
	// which=false -> 0x0e, true->0x0f.
	_Wire.beginTransmission(CLOCK_ADDRESS);
	if (which) {
		_Wire.write(0x0f);
	} else {
		_Wire.write(0x0e);
	}
	_Wire.write(control);
	_Wire.endTransmission();
}