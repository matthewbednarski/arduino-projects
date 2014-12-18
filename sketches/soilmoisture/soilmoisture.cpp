#include "soilmoisture.h"

/**
 * Sensor pins
 */
uint8_t SENSOR_0_PIN =  A1;    // Analog pin - sensor 0
uint8_t POWER_0_PIN  = A0;    // Digital pin used to powwer sensor 0


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
  // panstamp.goToSleep();
  panstamp.sleepWd(WDTO_2S);

  // Serial.println("looped s...");
  // Transmit sensor data
  getRegister(REGI_SENSOR)->getData();
  // Serial.println("looped...");
  // Transmit power voltage
  getRegister(REGI_VOLTSUPPLY)->getData();
  //Serial.println("looped 1...");

}

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


