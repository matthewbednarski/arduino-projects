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
  if( (time - last_time) > 500){
    if (irrecv.decode(&results)) {
      Serial.println(results.value, HEX);
      irrecv.resume(); // Receive the next value
      last_time = time;
    }

  }
}
