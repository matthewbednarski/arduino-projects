#include <Arduino.h>
#include "utils.h"

bool timeElapsed(int previous, int interval){
	return (millis() - previous ) >= interval;
}
