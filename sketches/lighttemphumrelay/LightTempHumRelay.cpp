#include <Arduino.h> 
#include <EEPROM.h>
#include "product.h"
#include "panstamp.h"
#include "regtable.h"
#include <dht11.h>

#define LEDRED  PD5
#define LEDGRN  PD3

const void updateVoltage(byte rId);
const void updateLightSensor(byte rId);
const void updateTempHumSensor(byte rId);
const void setSendSensorStates( byte rId, byte *state);
const void selectRelay( byte rId, byte *state);
const void switchRelay( byte rId, byte *state);
const void updateRelayStates(byte rId);
const void setSendRelayStates( byte rId, byte *state);

void readTempHum();

void setup();
void loop();

void setupOutPin(int p);
void addPinSet(uint8_t sensorPin, uint8_t mode, uint8_t pwr);
void setupSensorPin(uint8_t sensorPin, uint8_t mode, uint8_t pwr);
void setupRelayPin(uint8_t sPin);
void setupRelays();

void ledOff();
void ledRed();
void ledGreen();
void ledRedGreen();

void onRelay(uint8_t relay);
void offRelay(uint8_t relay);

uint8_t oPinsNum = 0;
uint8_t oPins[] = { 0, 0, 0, 0, 0};
void setupOutPin(int p){
  pinMode(p, OUTPUT);
  digitalWrite(p, LOW);
  oPins[oPinsNum] = p;
  oPinsNum++;
}

uint8_t sPinsNum = 0;
uint8_t sPins[] = {0,0,0,0,0,0,0,0};
uint8_t mPins[] = {0,0,0,0,0,0,0,0};
uint8_t pPins[] = {0,0,0,0,0,0,0,0}; 
void addPinSet(uint8_t sPin, uint8_t mode, uint8_t pwr){
  sPins[sPinsNum] = sPin;
  mPins[sPinsNum] = mode;
  pPins[sPinsNum] = pwr;
  sPinsNum++;
}
void setupSensorPin(uint8_t sPin, uint8_t mode, uint8_t pwr){
  //setup power pin
  pinMode(pwr, OUTPUT);
  //turn off power pin initially
  digitalWrite(pwr, LOW);

  pinMode(sPin, mode);
  if(mode == OUTPUT){  
    digitalWrite(sPin, HIGH);

  }else{
    //enable pull-up resistor
    digitalWrite(sPin, HIGH);   
  }
}

void setupRelayPin(uint8_t sPin){
  pinMode(sPin, OUTPUT);
  digitalWrite(sPin, LOW);
}
#define SENSOR_DHT_11 A0
#define SENSOR_DHT_11_PWR A1
#define SENSOR_DHT_11_MODE  INPUT


#define SENSOR_LIGHT  A4
#define SENSOR_LIGHT_PWR  A5
#define SENSOR_LIGHT_MODE   INPUT


#define SENSOR_RELAY  A3
#define SENSOR_RELAY_MODE   OUTPUT
#define SENSOR_RELAY_PWR  A2

static uint8_t relays[] = { A3, A2, A6, A7, PD3, PD5, PD6, PD7};
static uint8_t relays_states[] = { 0, 0, 0, 0, 0, 0 ,0, 0 };
static uint8_t relays_count = 8;
static uint8_t send_states = 0;
void setupRelays(){
  uint8_t count = relays_count;
  for(int i = 0; i < count; i++){
    setupRelayPin(relays[i]);
    relays_states[i] = 0;
  }
}
void onRelay(uint8_t relay){
  digitalWrite(relays[relay], HIGH);
  relays_states[relay] = 1;
}
void offRelay(uint8_t relay){
  digitalWrite(relays[relay], LOW);  
  relays_states[relay] = 0;
}

void ledOff(){
  digitalWrite(LEDRED, HIGH);  
  digitalWrite(LEDGRN, HIGH);  
}
void ledRed(){
  digitalWrite(LEDRED, LOW);  
  digitalWrite(LEDGRN, HIGH);  
}
void ledGreen(){
  digitalWrite(LEDRED, HIGH);  
  digitalWrite(LEDGRN, LOW);  
}
void ledRedGreen(){
  digitalWrite(LEDRED, LOW);  
  digitalWrite(LEDGRN, LOW);  
}

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{

  //Serial.begin(38400);
  //Serial.flush();

  setupOutPin(LEDGRN);
  setupOutPin(LEDRED);
  // Init panStamp

  setupRelays();
  addPinSet(SENSOR_DHT_11, SENSOR_DHT_11_MODE, SENSOR_DHT_11_PWR);
  addPinSet(SENSOR_RELAY, SENSOR_RELAY_MODE, SENSOR_RELAY_PWR);
  addPinSet(SENSOR_LIGHT, SENSOR_LIGHT_MODE, SENSOR_LIGHT_PWR);


  panstamp.init();
  panstamp.cc1101.setCarrierFreq(CFREQ_868);
  //panstamp.cc1101.setCarrierFreq(CFREQ_433);

  //Serial.println("LightTempHumRelay starting up...");


  // Transmit product code
  getRegister(REGI_PRODUCTCODE)->getData();

  // Enter SYNC state
  panstamp.enterSystemState(SYSTATE_SYNC);

  //Serial.println("\tListening for commands...");
  // During 3 seconds, listen the network for possible commands whilst the LED blinks
  for(int i=0 ; i<6 ; i++)
  {
    ledGreen();
    delay(1000);
    ledOff();
    delay(1000);
    ledRed();
    delay(1000);
    ledOff();
    delay(3000);
  }

  // Switch to Rx OFF state
  panstamp.enterSystemState(SYSTATE_RXON);

  //Serial.print("Product Code: ");
  //Serial.println(getRegister(REGI_PRODUCTCODE)->value[8], DEC);

  //Serial.println("Registers: ");
  //Serial.print("Volt supply: ");
  //Serial.println(REGI_O_VOLTSUPPLY, DEC);
  //Serial.print("Light Sensor: ");
  //Serial.println(REGI_O_SENSOR_LIGHT, DEC);
  //Serial.print("Temp/Hum Sensor: ");
  //Serial.println(REGI_O_SENSOR_TEMP_HUM, DEC);
  //Serial.print("Request Sensor States: ");
  //Serial.println(REGI_I_SEND_SENSOR_STATES, DEC);
  //Serial.print("Relays Register: ");
  //Serial.println(REGI_I_RELAYSELECT, DEC);
  //Serial.print("Relays On/Off Register: ");
  //Serial.println(REGI_I_RELAYSWITCH, DEC);
  //Serial.print("Relays States: ");
  //Serial.println(REGI_O_RELAYS_STATES, DEC);
  //Serial.print("Request Relays States: ");
  //Serial.println(REGI_I_SEND_RELAYS_STATES, DEC);

}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  readTempHum();
  if(send_states == 1){
    getRegister(REGI_O_RELAYS_STATES)->getData();
    send_states = 0;
    ledOff();
  }
  //getRegister(REGI_O_VOLTSUPPLY)->getData();
  delay(200);
  //getRegister(REGI_O_SENSOR_LIGHT)->getData();
  delay(200);
  //getRegister(REGI_O_SENSOR_TEMP_HUM)->getData();
  //delay(200);
  //getRegister(REGI_O_RELAYS_STATES)->getData();
  //delay(1000);
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
  static unsigned long voltage = 3300;
  static byte dtVoltage[2];
  REGISTER regVoltage(dtVoltage, sizeof(dtVoltage), &updateVoltage, NULL);

  static byte dtSensor[2];
  REGISTER regSensorLight(dtSensor, sizeof(dtSensor), &updateLightSensor, NULL);

  // Sensor value register (dual sensor)
  static byte dtSensorTempHum[4];
  REGISTER regSensorTempHum(dtSensorTempHum, sizeof(dtSensorTempHum), &updateTempHumSensor, NULL);

  static byte dtSendSensorStates[1];
  REGISTER regSendSensorStates(dtSendSensorStates, sizeof(dtSendSensorStates), NULL, &setSendSensorStates);

  static byte dtRelay[1];
  REGISTER regRelaySelect(dtRelay, sizeof(dtRelay), NULL, &selectRelay );

  static byte dtRelaySwitch[1];
  REGISTER regRelaySwitch(dtRelaySwitch, sizeof(dtRelaySwitch), NULL, &switchRelay );

  static byte dtRelayStates[8];
  REGISTER regRelayStates(dtRelayStates, sizeof(dtRelayStates), &updateRelayStates, NULL);

  static byte dtSendRelayStates[1];
  REGISTER regSendRelayStates(dtSendRelayStates, sizeof(dtSendRelayStates), NULL, &setSendRelayStates);

  /**
   * Initialize table of registers
   */
DECLARE_REGISTERS_START()
  // Put here pointers to your custom registers
  &regVoltage,
  &regSensorLight,
  &regSensorTempHum,
  &regSendSensorStates,
  &regRelaySelect,
  &regRelaySwitch,
  &regRelayStates,
  &regSendRelayStates,
  DECLARE_REGISTERS_END()

  /**
   * Definition of common getter/setter callback functions
   */
DEFINE_COMMON_CALLBACKS()

  /**
   * Definition of custom getter/setter callback functions
   */



const void updateVoltage(byte rId)
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
  uint8_t voltageSupply = result;     // Update global variable Vcc

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
  //return 0;
}
const void updateLightSensor(byte rId)
{
  //setupSensorPin(SENSOR_LIGHT,SENSOR_LIGHT_MODE, SENSOR_LIGHT_PWR  );
  pinMode(SENSOR_LIGHT_PWR, OUTPUT);
  digitalWrite(SENSOR_LIGHT_PWR, LOW);

  pinMode(SENSOR_LIGHT, INPUT);
  digitalWrite(SENSOR_LIGHT_PWR, HIGH);
  delay(200);

  int lumin = analogRead(SENSOR_LIGHT);
  digitalWrite(SENSOR_LIGHT_PWR, LOW);
  int val = map(lumin, 0, 1023, 0, 100);


  dtSensor[0] = (val >> 8) & 0xFF;
  dtSensor[1] = val & 0xFF;
  //return 0;
}
void readTempHum(){
  int temperature, humidity, chk;
  int pwr = SENSOR_DHT_11_PWR;
  int sPin = SENSOR_DHT_11;
  int mode = SENSOR_DHT_11_MODE;
  //setupSensorPin(SENSOR_DHT_11,SENSOR_DHT_11_MODE, SENSOR_DHT_11_PWR  );

  //setup power pin
  pinMode(pwr, OUTPUT);
  //turn off power pin initially
  digitalWrite(pwr, LOW);

  pinMode(sPin, mode);
  //enable pull-up resistor
  //digitalWrite(sPin, HIGH);   

  ledRedGreen();
  dht11 DHT;
  digitalWrite(SENSOR_DHT_11_PWR, HIGH);
  delay(1500);

  chk = DHT.read(sPin);
  digitalWrite(SENSOR_DHT_11_PWR, LOW);
  if(chk != DHTLIB_OK){
    int tmp = chk;

    ledRed();
    return;
  }
  ledGreen();

  temperature = DHT.temperature * 10 + 500;
  humidity = DHT.humidity * 10;

  dtSensorTempHum[0] = (temperature >> 8) & 0xFF;
  dtSensorTempHum[1] = temperature & 0xFF;
  dtSensorTempHum[2] = (humidity >> 8) & 0xFF;
  dtSensorTempHum[3] = humidity & 0xFF;

/*
  //Serial.print("Temp: ");
  //Serial.println( temperature );
  Serial.print("Temp hex: ");
  Serial.print(dtSensorTempHum[0] , HEX);
  Serial.print(dtSensorTempHum[1] , HEX);
  Serial.println();
  Serial.print("Hum: ");
  Serial.println( humidity );
  Serial.print("Hum hex: ");
  Serial.print(dtSensorTempHum[2] , HEX);
  Serial.print(dtSensorTempHum[3] , HEX);
  Serial.println();
  Serial.println();
*/

}
const void updateTempHumSensor(byte rId)
{
  readTempHum();
}

const void setSendSensorStates( byte rId, byte *state)
{

  byte i;
  memcpy( dtSendSensorStates, state, sizeof(dtSendSensorStates));
  uint8_t req = dtSendSensorStates[0];

  if(req == 1){
    // Transmit relays states
    delay(20);
    getRegister(REGI_O_VOLTSUPPLY)->getData();
    getRegister(REGI_O_SENSOR_LIGHT)->getData();
    getRegister(REGI_O_SENSOR_TEMP_HUM)->getData();
    delay(2000);
  }
  //return 0;
}
const void selectRelay( byte rId, byte *s)
{
  byte i;
  memcpy( dtRelay, s, sizeof(dtRelay));
  //return 0;
}
const void switchRelay( byte rId, byte *s)
{
  byte i;
  uint8_t last, next, relay;

  ledRedGreen();
  last = dtRelaySwitch[0];
  memcpy( dtRelaySwitch, s, sizeof(dtRelaySwitch));
  next = dtRelaySwitch[0];

  relay = dtRelay[0];
  if(next == 1){
        onRelay(relay);
  }else if(next == 0){
        offRelay(relay); 
  }
  delay(100);
  ledOff();
}
const void updateRelayStates(byte rId)
{  
  uint8_t relays_count = 8;
  for(int i = 0; i < relays_count; i++){
    dtRelayStates[i] = relays_states[i];  
  }   
}
const void setSendRelayStates( byte rId, byte *state)
{
  byte i;
  uint8_t last, req;
  last = dtSendRelayStates[0];

  memcpy( dtSendRelayStates, state, sizeof(dtSendRelayStates));
  req = dtSendRelayStates[0];

  if(req == 1){
    ledRedGreen();
    send_states = 1;
  }
}
