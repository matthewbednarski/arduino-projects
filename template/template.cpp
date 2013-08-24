#include <Arduino.h> 
#include <EEPROM.h>
#include "product.h"
#include "panstamp.h"
#include "regtable.h"


void setup();
void loop();


/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{

	Serial.begin(9600);
	// Init panStamp
	panstamp.init();
	Serial.println("Template starting up...");


	// Transmit product code
	getRegister(REGI_PRODUCTCODE)->getData();
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
	Serial.print("Product Code: ");
	Serial.println(REGI_PRODUCTCODE);
	getRegister(REGI_PRODUCTCODE)->getData();
	delay(400);
}

/**
 * Declaration of common callback functions
 */
DECLARE_COMMON_CALLBACKS()

	/**
	 * Definition of common registers
	 */
DEFINE_COMMON_REGISTERS()

	/*
	 * Definition of custom registers
	 */
	// Your custom registers come here

	/**
	 * Initialize table of registers
	 */
DECLARE_REGISTERS_START()
	// Put here pointers to your custom registers
DECLARE_REGISTERS_END()

	/**
	 * Definition of common getter/setter callback functions
	 */
DEFINE_COMMON_CALLBACKS()

	/**
	 * Definition of custom getter/setter callback functions
	 */

