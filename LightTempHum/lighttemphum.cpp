#include <Arduino.h>
#include <EEPROM.h>
#include "product.h"
#include "regtable.h"
#include "panstamp.h"
#include <dht11.h>

#define TRACE
/**
 * Uncomment if you are reading Vcc from A7. All battery-boards do this
 */
//#define VOLT_SUPPLY_A7

/**
 * LED pin
 */
#define LEDPIN           4
#define SERIAL_BAUD	38400
/**
 * Sensor pins
 */
#define SENSOR_0_PIN  3     // Analog pin 3 = Digital pin 17- sensor 0
#define POWER_0_PIN   16     // Digital pin 16 = Analog pin 2
#define SENSOR_1_PIN  19     // Analog pin 5 = Digital pin 19- sensor 0
#define POWER_1_PIN   18     // Digital pin 18 = Analog pin 4

#define BUTTON_PIN     8    // Digital pin used to powwer sensor 1

void setup();
void loop();
void initPin(unsigned int pin, unsigned int mode, unsigned int low_high);
const void updateVoltSupply(byte rId);
const void updateLightSensor(byte rId);
const void updateTempHumSensor(byte rId);
int sensorReadTempHum(unsigned int dht_11_data_pin, unsigned int dht_11_power_pin);


/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
	int i;
	Serial.begin(SERIAL_BAUD);
	Serial.print("Starting up at serial baud: ");
	Serial.println(SERIAL_BAUD, DEC);


	// Initialize power pins LIGHT sensor
	initPin(POWER_0_PIN, OUTPUT, LOW);
	// Initialize power pins DHT11 sensor
	initPin(POWER_1_PIN, OUTPUT, LOW);



	// Initialize pin for BUTTON
	initPin(BUTTON_PIN, INPUT, HIGH);
	// Initialize pin for LED
	initPin(LEDPIN, OUTPUT, HIGH);


	// Init panStamp
	panstamp.init();

	panstamp.cc1101.setCarrierFreq(CFREQ_433);
	// Transmit product code
	getRegister(REGI_PRODUCTCODE)->getData();
	getRegister(REGI_PRODUCTCODE)->getData();
	delay(20);

	// Enter SYNC state
	panstamp.enterSystemState(SYSTATE_SYNC);

	Serial.println("\tListening for commands...");
	// During 3 seconds, listen the network for possible commands whilst the LED blinks
	for(i=0 ; i<6 ; i++)
	{
		digitalWrite(LEDPIN, HIGH);
		delay(100);
		digitalWrite(LEDPIN, LOW);
		delay(100);
	}

	// Transmit periodic Tx interval
	//getRegister(REGI_TXINTERVAL)->getData();
	//delay(20);
		// Transmit power voltage
	//getRegister(REGI_VOLTSUPPLY)->getData();
	//delay(20);



	Serial.print("Product Code: ");
	Serial.println(*getRegister(REGI_PRODUCTCODE)->value, DEC);
	
	Serial.println("Registers: ");
	Serial.print("Volt supply: ");
	Serial.println(REGI_VOLTSUPPLY, DEC);
	Serial.print("Light Sensor: ");
	Serial.println(REGI_SENSOR_LIGHT, DEC);
	Serial.print("Temp/Hum Sensor: ");
	Serial.println(REGI_SENSOR_TEMP_HUM, DEC);

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
	int val = digitalRead(BUTTON_PIN);
	if(val < 1){
		digitalWrite(LEDPIN, HIGH);
#ifdef TRACE
		Serial.print("Button ");
		Serial.print(BUTTON_PIN, DEC);
		Serial.println(" pressed.");
#endif
		// Transmit sensor data
		getRegister(REGI_SENSOR_LIGHT)->getData();
		// Transmit sensor data
		getRegister(REGI_SENSOR_TEMP_HUM)->getData();
		// Transmit power voltage
		getRegister(REGI_VOLTSUPPLY)->getData();

/*#ifdef TRACE
		Serial.print("Sensor value: ");
		Serial.println(*getRegister(REGI_SENSOR_LIGHT)->value, DEC);

		Serial.print("Sensor value: ");
		Serial.println(*getRegister(REGI_SENSOR_TEMP_HUM)->value, DEC);

		Serial.print("Volt supply: ");
		Serial.println(*getRegister(REGI_VOLTSUPPLY)->value, DEC);
#endif*/
		// Sleep
		panstamp.goToSleep();
		digitalWrite(LEDPIN, LOW);
	}

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
	REGISTER regVoltSupply(dtVoltSupply, sizeof(dtVoltSupply), &updateVoltSupply, NULL);
	// Sensor value register (dual sensor)
	static byte dtSensor[2];
	REGISTER regSensorLight(dtSensor, sizeof(dtSensor), &updateLightSensor, NULL);

	// Sensor value register (dual sensor)
	static byte dtSensorTempHum[4];
	REGISTER regSensorTempHum(dtSensorTempHum, sizeof(dtSensorTempHum), &updateTempHumSensor, NULL);
	/**
	 * Initialize table of registers
	 */
DECLARE_REGISTERS_START()
	&regVoltSupply,
	&regSensorLight,
	&regSensorTempHum
DECLARE_REGISTERS_END()

	/**
	 * Definition of common getter/setter callback functions
	 */
DEFINE_COMMON_CALLBACKS()

	/**
	 * Definition of custom getter/setter callback functions
	 */


	/**
	 * sensor_ReadTempHum
	 *
	 * Read temperature and humidity values from DHT11 sensor
	 *
	 * Return -1 in case of error. Return 0 otherwise
	 */
int sensorReadTempHum(unsigned int dht_11_data_pin, unsigned int dht_11_power_pin)
{
	int temperature, humidity, chk;


	digitalWrite(dht_11_power_pin, HIGH);
	dht11 sensor;

	delay(1500);   
	chk = sensor.read(dht_11_data_pin);

	digitalWrite(dht_11_power_pin, LOW);

	if (chk != DHTLIB_OK){
#ifdef TRACE
		Serial.print("Got DHT11 lib: ");
		Serial.println(DHTLIB_OK, DEC);
#endif
		return -1;
	}

	temperature = sensor.temperature * 10 + 500;
	humidity = sensor.humidity * 10;

#ifdef TRACE
	Serial.print("Temp is: ");
	Serial.print(sensor.temperature, DEC);
	Serial.println("°C");
	Serial.print("Humidity percent: ");
	Serial.print(sensor.humidity, DEC);
	Serial.println("%");
#endif

	dtSensorTempHum[0] = (temperature >> 8) & 0xFF;
	dtSensorTempHum[1] = temperature & 0xFF;
	dtSensorTempHum[2] = (humidity >> 8) & 0xFF;
	dtSensorTempHum[3] = humidity & 0xFF;

	return 0;
}

/**
 * updateVoltSupply
 *
 * Measure voltage supply and update register
 *
 * 'rId'  Register ID
 */
const void updateVoltSupply(byte rId)
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
	 * no other register is going to use "updateVoltSupply" as "updater" function
	 */

	// Update register value
	regTable[rId]->value[0] = (result >> 8) & 0xFF;
	regTable[rId]->value[1] = result & 0xFF;
}


/**
 * updateLightSensor
 *
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updateLightSensor(byte rId)
{
	// Power sensors
	digitalWrite(POWER_0_PIN, HIGH);
	delay(10);

	// Read analog values
	unsigned int adcValue0 = analogRead(SENSOR_0_PIN);
	int val = map(adcValue0, 0, 1023, 0, 100);

#ifdef TRACE
	Serial.print("Light Sensor value is: ");
	Serial.println(val, DEC);
#endif

	// Unpower sensors
	digitalWrite(POWER_0_PIN, LOW);

	// Update register value
	dtSensor[0] = (val >> 8) & 0xFF;
	dtSensor[1] = val & 0xFF;
}

/**
 * updateTempHumSensor
 *
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updateTempHumSensor(byte rId)
{
	sensorReadTempHum(SENSOR_1_PIN, POWER_1_PIN);
}

void initPin(unsigned int pin, unsigned int mode, unsigned int low_high)
{
	pinMode(pin, mode);
	digitalWrite(pin, low_high);
}
