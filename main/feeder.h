/*
 * feeder.h
 *  Created on: 06 MAY 2022
 *   Author: NirK12
 *
 *  
*/
#ifndef _FEEDER_H    // Put these two lines at the top of your file.
#define _FEEDER_H    // (Use a suitable name, usually based on the file name


//SETUP I/O pins
void FEEDER_init(void);

//return Bool value for litchi out or not
bool FEEDER_get_litchi(int speed = 18);

/*
To use this function we must use in 10K resistor and Photoresistor 
The function set by 8 level of light between 1 to 8 (Zero also can be but in Egypt dark)
Connect the wire between the resistor to the Photoresistor 
*/
int FEEDER_light_sensor(void);

/*OptoSwich is an litchi OptoSwitch etactor
To use this function we must use in 10K resistor for the emitter and 
for the detector is an 100 ohm resistor
*/
bool FEEDER_optoswitch(int threshold = 750);

//Replace the the feeder position to the start
void FEEDER_replace(void);


	
//Relay to turn off/on the components
void FEEDER_relay(bool action);

void FEEDER_led_err(int errorType );



#endif
