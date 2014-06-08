/*
 * Author: Daniel Berenguer
 * Creation date: 04/29/2013
 *
 * Device:
 * Soil Moisture sensor
 *
 * Description:
 * This application measures soil moisture from any two sensor providing an
 * analog signal
 *
 * These devices are low-power enabled so they will enter low-power mode
 * just after reading the sensor values and transmitting them over the
 * SWAP network.
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * soilmoisture.xml
 */
 
#include <Arduino.h>
#include <EEPROM.h>
#include "product.h"
#include "regtable.h"
#include "panstamp.h"

/**
 * Uncomment if you are reading Vcc from A7. All battery-boards do this
 */
#define VOLT_SUPPLY_A7

/**
 * LED pin
 */
#define LEDPIN        4

/**
 * Sensor pins
 */
uint8_t SENSOR_0_PIN =  A1;    // Analog pin - sensor 0
uint8_t POWER_0_PIN  = A0;    // Digital pin used to powwer sensor 0
//uint8_t SENSOR_1_PIN = A3;    // Analog pin - sensor 1
//uint8_t POWER_1_PIN  = A2;  // Digital pin used to powwer sensor 1

void setup();
void loop();
const void updtVoltSupply(byte rId);
const void updtSensor(byte rId);
/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  int i;
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  // Initialize power pins
  pinMode(POWER_0_PIN, OUTPUT);
  digitalWrite(POWER_0_PIN, LOW);
  //pinMode(POWER_1_PIN, OUTPUT);
  //digitalWrite(POWER_1_PIN, LOW);

  // Init panStamp
  panstamp.init();

  panstamp.cc1101.setCarrierFreq(CFREQ_868);
  //panstamp.cc1101.setCarrierFreq(CFREQ_433);

  // Transmit product code
  getRegister(REGI_PRODUCTCODE)->getData();

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

  // Transmit periodic Tx interval
  getRegister(REGI_TXINTERVAL)->getData();
  // Transmit power voltage
  getRegister(REGI_VOLTSUPPLY)->getData();
   // Switch to Rx OFF state
  panstamp.enterSystemState(SYSTATE_RXOFF);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  getRegister(REGI_TXINTERVAL)->getData();
  // Sleep
  panstamp.goToSleep();
  //panstamp.sleepWd(WDTO_2S);
  Serial.println("asleep...");

  Serial.println("looped s...");
  // Transmit sensor data
  getRegister(REGI_SENSOR)->getData();
  Serial.println("looped...");
  // Transmit power voltage
  getRegister(REGI_VOLTSUPPLY)->getData();
  Serial.println("looped 1...");

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
// Voltage supply
static unsigned long voltageSupply = 3300;
static byte dtVoltSupply[2];
REGISTER regVoltSupply(dtVoltSupply, sizeof(dtVoltSupply), &updtVoltSupply, NULL);
// Sensor value register (dual sensor)
static byte dtSensor[4];
REGISTER regSensor(dtSensor, sizeof(dtSensor), &updtSensor, NULL);

/**
 * Initialize table of registers
 */
DECLARE_REGISTERS_START()
  &regVoltSupply,
  &regSensor
DECLARE_REGISTERS_END()

/**
 * Definition of common getter/setter callback functions
 */
DEFINE_COMMON_CALLBACKS()

/**
 * Definition of custom getter/setter callback functions
 */
 
/**
 * updtVoltSupply
 *
 * Measure voltage supply and update register
 *
 * 'rId'  Register ID
 */
const void updtVoltSupply(byte rId)
{  
  unsigned long result;
  
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  voltageSupply = result;     // Update global variable Vcc
  
  #ifdef VOLT_SUPPLY_A7
  
  // Read voltage supply from A7
  unsigned short ref = voltageSupply;
  result = analogRead(7);
  result *= ref;
  result /= 1024;
  #endif

  /**
   * register[eId]->member can be replaced by regVoltSupply in this case since
   * no other register is going to use "updtVoltSupply" as "updater" function
   */

  // Update register value
  regTable[rId]->value[0] = (result >> 8) & 0xFF;
  regTable[rId]->value[1] = result & 0xFF;
  //dtVoltSupply[0] = 0;
  //dtVoltSupply[1] = 0;
}

/**
 * updtSensor
 *
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updtSensor(byte rId)
{
  // Power sensors
  digitalWrite(POWER_0_PIN, HIGH);
  //digitalWrite(POWER_1_PIN, HIGH);
  delay(10);
  // Read analog values
  unsigned int adcValue0 = analogRead(SENSOR_0_PIN);
  unsigned int adcValue1 = 0;
  //unsigned int adcValue1 = analogRead(SENSOR_1_PIN);
  // Unpower sensors
  digitalWrite(POWER_0_PIN, LOW);
  //digitalWrite(POWER_1_PIN, LOW);
  
  // Update register value
  dtSensor[0] = (adcValue0 >> 8) & 0xFF;
  dtSensor[1] = adcValue0 & 0xFF;
  dtSensor[2] = 0xFF;
  dtSensor[3] = 0xFF;
}


