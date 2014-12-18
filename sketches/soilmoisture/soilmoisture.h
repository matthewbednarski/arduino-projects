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

void setup();
void loop();
const void updtVoltSupply(byte rId);
const void updtSensor(byte rId);
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
