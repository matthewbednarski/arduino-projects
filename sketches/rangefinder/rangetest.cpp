#include <Arduino.h> 

#define LEDPIN 13

#define RNG_PWR 2
#define RNG_TRG 3
#define RNG_ECH 4

void setup();
void loop();
void prepareSensor();
long readSensor();
long microToInch(long micro);
long microToCm(long micro);

void prepareSensor()
{
  
  pinMode(RNG_PWR, OUTPUT);
  digitalWrite(RNG_PWR, LOW);
  pinMode(RNG_TRG, OUTPUT);
  digitalWrite(RNG_TRG, LOW);
  pinMode(RNG_ECH, INPUT);

}
long readSensor()
{
   long r;
   digitalWrite(LEDPIN, HIGH);
   digitalWrite(RNG_PWR, HIGH);
   delay(20);
   digitalWrite(RNG_TRG, HIGH);
   delayMicroseconds(5);
   digitalWrite(RNG_TRG, LOW);
   r = pulseIn(RNG_ECH, HIGH);

   
   digitalWrite(LEDPIN, LOW);
   return r * 10;
}
long microToInch(long micro)
{
  return (((micro / 74) / 2) / 10);
}
long microToCm(long micro)
{
  return (((micro / 29) / 2) / 10);
}


void setup()
{

  uint8_t i;
	Serial.begin(38400);
	// Init panStamp
  pinMode(LEDPIN, OUTPUT);

	//panstamp.cc1101.setCarrierFreq(CFREQ_433);
	Serial.println("rangetest starting up...");

  prepareSensor();
	// During 3 seconds, listen the network for possible commands whilst the LED blinks
	for(i=0 ; i<6 ; i++)
	{
		digitalWrite(LEDPIN, HIGH);
		delay(100);
		digitalWrite(LEDPIN, LOW);
		delay(400);
	}

}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  
  long d = readSensor();
  Serial.print( "dist: ");
  Serial.print( microToCm(d) );
  Serial.println( " cm ");
  delay(100);

}
