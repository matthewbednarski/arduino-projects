
#ifndef _REGTABLE_H
#define _REGTABLE_H

#include "Arduino.h"
#include "register.h"
#include "commonregs.h"

/**
 * Register indexes
 */
DEFINE_REGINDEX_START()
  REGI_VOLTSUPPLY,
  REGI_SENSOR_LIGHT,
  REGI_SENSOR_TEMP_HUM
DEFINE_REGINDEX_END()


struct t_sensor {
  int* sensors;
  int power;
};

#endif

