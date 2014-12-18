
#include <Arduino.h> 
#include "reading.h"
#include <digitalWriteFast.h>
#include <EEPROM.h>
#include <utils.h>
#include "product.h"
#include "panstamp.h"
#include "regtable.h"


reading light_data;

#define LIGHT_LOW_LIMIT 80

#define A5_PHOTO A5
int A5_MIN  = 100;
int A5_MAX = 100;
#define A5_READS 10
int A5_READS_OVER = 0;
int A5_READS_UNDER = 0;

int A5_LIGHT = 0;

int INTERVAL_MILLIS = 500;
int PREVIOUS_MILLIS = 0;

#define D3_RELAY 3
#define D3_ON	 0

void setup();
void loop();


void setup_photocell();
void loop_photocell();

void setup_relay();
void loop_relay();

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
	setup_photocell();
	setup_relay();
	Serial.begin(9600);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  DDRB = 1 << 12;

  bool can_loop = timeElapsed(PREVIOUS_MILLIS, INTERVAL_MILLIS);
	PREVIOUS_MILLIS = millis();
	if(can_loop){
		loop_photocell();
		loop_relay();
	}
}


void setup_photocell(){
	//set pin A5 to INPUT
	pinModeFast(A5_PHOTO, INPUT);
	A5_MAX = analogRead(A5_PHOTO);
}

void loop_photocell(){
	int current = analogRead(A5_PHOTO);
	if(current < A5_MIN){
		A5_MIN = current;
	}else if(current > A5_MAX){
		A5_MAX = current;
	}
	A5_LIGHT = map(current, A5_MIN, A5_MAX, 0, 100);
	if (A5_LIGHT < LIGHT_LOW_LIMIT){
		if(A5_READS_UNDER > 0){
			A5_READS_UNDER--;	
		}
		if(A5_READS_OVER < 10){
			A5_READS_OVER = 10;
		}
	}else{
		if(A5_READS_OVER > 0){
			A5_READS_OVER--;
		}
		if(A5_READS_UNDER < 10){
			A5_READS_UNDER = 10;
		}
	}
#ifdef TRACE 
	Serial.print("Light: ");
	Serial.println(A5_LIGHT);
#endif
}

void setup_relay(){
	//set pin D3 to OUTPUT
	pinModeFast(D3_RELAY, OUTPUT);
}

void loop_relay(){
	if(A5_READS_UNDER == 0){
		digitalWrite(D3_RELAY, HIGH);
	}else if(A5_READS_OVER == 0){
		digitalWrite(D3_RELAY, LOW);
	}
}
