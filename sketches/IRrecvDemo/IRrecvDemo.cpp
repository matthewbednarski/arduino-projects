#include "IRrecvDemo.h"

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  //delay(100);
  static unsigned long last_time;
  unsigned long time = millis();
  if( (time - last_time) > 100){
    if (irrecv.decode(&results)) {
      Serial.print("# len: ");
      Serial.print(results.rawlen, DEC);
      Serial.print(" -> value: ");
      Serial.println(*results.rawbuf, DEC);
      Serial.print("Decoded: ");
      Serial.print(results.value, DEC);
      Serial.print(" -> Type: ");
      Serial.println(results.decode_type, DEC);
      irrecv.resume(); // Receive the next value
      last_time = time;
    }

  }
}
