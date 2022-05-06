/*
 * feeder.cpp
 *  Created on: 06 MAY 2022
 *   Author: NirK12
 *
 *  
*/
#include <stdio.h>
#include <Arduino.h>
#include "Feeder.h"
#include <virtuabotixRTC.h> 


//Set the pins for the arduino:
//pins for Stepper motor
#define	STEP_PIN 3
#define	DIR_PIN	2

//Sleep & Rset pins is need to be connected to all the drivers that request it and are normally on. (stepper, RFID and etc..)
#define	SLEEP_PIN 4
#define	RSET_PIN 5
#define RELAY_PIN NULL
#define	LED_ERR_PIN	NULL

//pin for Opto-Swich
int const OPTO_SWITCH_PIN = A1;
int const LIGHT_SENSOR_PIN = A0;

//pin for Read switch
int const READ_SWITCH_PIN = 6;

//Stepper setup loctions. 200 steps is a one cycle
int const STEPS_TO_OPEN = 60;
int const STEPS_TO_CLOSE = 40; 
int const OS_THRESHOLD = 750; //Threshold for optoswich
unsigned long TIME_INTERVAL_REPLACE = 3000; //Time that replace is running




void FEEDER_init(void){
	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);
	pinMode(SLEEP_PIN, OUTPUT);
	pinMode(RSET_PIN, OUTPUT);
	pinMode(OPTO_SWITCH_PIN, INPUT);
	pinMode(READ_SWITCH_PIN,INPUT);
	pinMode(LIGHT_SENSOR_PIN, INPUT);
}




//speed value is between 0 to 20 (0 slow, 20 high speed)
bool FEEDER_get_litchi(int speed = 18){
	bool LitchiFlag = false; //Induction of the litchi getting out
	
	//Check Speed Value is in the correct range
	if (speed > 20 || speed < 0){
		 Serial.print("Invalid Speed value, Speed Value is need to be between 0 to 20!!");
		return false;
	}
	speed = (16383 - 800*speed);

	for(int Try = 0; Try < 2; Try++){
	digitalWrite(DIR_PIN,HIGH); // Enables the motor to move in clock direction
	// Makes 200 pulses for making one full cycle rotation
		
		for(int x = 0; x < STEPS_TO_OPEN; x++){
		  digitalWrite(STEP_PIN,HIGH); 
		  delayMicroseconds(speed);	  
		  digitalWrite(STEP_PIN,LOW); 
		  LitchiFlag = analogRead(OPTO_SWITCH_PIN);
		  delayMicroseconds(speed);
		  if(LitchiFlag >= OS_THRESHOLD){
			for(int x = 0; x < STEPS_TO_CLOSE ; x++){ //close the hole aftter the litchi got out
			  digitalWrite(STEP_PIN,HIGH); 
			  delayMicroseconds(400);	  
			  digitalWrite(STEP_PIN,LOW); 
			  LitchiFlag = analogRead(OPTO_SWITCH_PIN);
			  delayMicroseconds(400);
			} 
			return true;
		  }
		}
		delay(500);
			digitalWrite(DIR_PIN,LOW); //Changes the rotations direction
		  // Makes few pulses for making that the litchi or the stepper motor not stack
		for(int x = 0; x < 7; x++){
		  digitalWrite(STEP_PIN,HIGH);
		  delayMicroseconds(speed);
		  digitalWrite(STEP_PIN,LOW);
		  LitchiFlag = analogRead(OPTO_SWITCH_PIN);
		  delayMicroseconds(speed);
		  if(LitchiFlag >= OS_THRESHOLD){
			  for(int x = 0; x < STEPS_TO_CLOSE; x++){//close the hole aftter the litchi got out
			  digitalWrite(STEP_PIN,HIGH); 
			  delayMicroseconds(400);	  
			  digitalWrite(STEP_PIN,LOW); 
			  LitchiFlag = analogRead(OPTO_SWITCH_PIN);
			  delayMicroseconds(400);
			  }
			return true;
		  }
		}
	}
	return false;
}




int FEEDER_light_sensor(void){
  int value = analogRead(LIGHT_SENSOR_PIN);
  value = value/128;
  return value;
}




bool FEEDER_optoswitch(int threshold = 750){
	int analogValue = analogRead(OPTO_SWITCH_PIN);
	if(analogValue > threshold){
		return true;
	}
	else
		return false;
}




void FEEDER_replace(void){
	digitalWrite(DIR_PIN,HIGH);
	unsigned long previousTime = millis();
	while(true){	
		unsigned long currentTime = millis();
		// task 1
		if (digitalRead(READ_SWITCH_PIN) != HIGH || (currentTime - previousTime > TIME_INTERVAL_REPLACE)){
			previousTime = currentTime;
			return;
		}
		digitalWrite(STEP_PIN,HIGH); 
		delayMicroseconds(1000);	  
		digitalWrite(STEP_PIN,LOW); 
		delayMicroseconds(1000);
	}		
}




void FEEDER_relay(bool action){
	if (action){
		digitalWrite(RELAY_PIN, HIGH);
	}
	else
		digitalWrite(RELAY_PIN, LOW);
}




void FEEDER_led_err(int errType ){
	switch (errType) {
	  case 1:
		//
		digitalWrite(RELAY_PIN, HIGH);
		delay(5000);
		digitalWrite(RELAY_PIN, LOW);
		break;
	  case 2:
		for(int i = 0; i > 10; i++){
			digitalWrite(RELAY_PIN, HIGH);
			delay(1000);
			digitalWrite(RELAY_PIN, LOW);
			delay(1000);
		}
		break;
	  case 3:
		for(int i = 0; i > 20; i++){
			digitalWrite(RELAY_PIN, HIGH);
			delay(300);
			digitalWrite(RELAY_PIN, LOW);
			delay(300);
		}
		break;
	}
	return;
}

