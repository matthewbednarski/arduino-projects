#include <Arduino.h> 
#include <EEPROM.h>
#include "product.h"
#include "panstamp.h"
#include "regtable.h"

#define pcEnableInterrupt()     PCICR = 0x07    // Enable Pin Change interrupts on all ports
#define pcDisableInterrupt()    PCICR = 0x00    // Disable Pin Change interrupts

void setup();
void loop();


/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{

	Serial.begin(38400);
	// Init panStamp
	panstamp.init();
	panstamp.cc1101.setCarrierFreq(CFREQ_433);
  panstamp.setTxInterval(tx_interval, 1);
	Serial.println("Template starting up...");

  // set device address in sketch
  //panstamp.cc1101.devAddress = 0xFFFF

	// Enter SYNC state
	panstamp.enterSystemState(SYSTATE_SYNC);

	// During 3 seconds, listen the network for possible commands whilst the LED blinks
	for(i=0 ; i<6 ; i++)
	{
		digitalWrite(LEDPIN, HIGH);
		delay(100);
		digitalWrite(LEDPIN, LOW);
		delay(400);
	}

	// Transmit product code
	getRegister(REGI_PRODUCTCODE)->getData();
	// Transmit periodic Tx interval
	getRegister(REGI_TXINTERVAL)->getData();
	// Transmit power voltage
	getRegister(REGI_VOLTSUPPLY)->getData();

	// Switch to Rx OFF state
	panstamp.enterSystemState(SYSTATE_RXOFF);

	// Enable Pin Change Interrupts
	pcEnableInterrupt();
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
	// Sleep for panstamp.txInterval seconds (register 10)
	panstamp.goToSleep();

	pcDisableInterrupt();

	Serial.print("Product Code: ");
	Serial.println(REGI_PRODUCTCODE);
	getRegister(REGI_PRODUCTCODE)->getData();
	delay(200);
	if (pcIRQ)
	{
		switch(updateValues())
		{
			case 2:
				// Transmit counter values
				getRegister(REGI_COUNTERS)->getData();
			case 1:
				// Transmit binary states
				getRegister(REGI_BININPUTS)->getData();
				break;
			default:
				break;
		}
		//Ready to receive new PC interrupts
		pcIRQ = false;
	}
	else
	{    
		// Just send states and counter values periodically, according to the value
		// of panstamp.txInterval (register 10)
		getRegister(REGI_COUNTERS)->getData();
		getRegister(REGI_BININPUTS)->getData();
	}

	pcEnableInterrupt();
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

