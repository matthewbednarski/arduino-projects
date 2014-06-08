#ifndef _REGTABLE_H
#define _REGTABLE_H

#include "Arduino.h"
#include "register.h"
#include "commonregs.h"

/**
 * Register indexes
 */
DEFINE_REGINDEX_START()
  REGI_O_VOLTSUPPLY,
  REGI_O_SENSOR_LIGHT,
  REGI_O_SENSOR_TEMP_HUM,
  REGI_I_SEND_SENSOR_STATES,
  REGI_I_RELAYSELECT,
  REGI_I_RELAYSWITCH,
  REGI_O_RELAYS_STATES,
  REGI_I_SEND_RELAYS_STATES
  // First index here = 11
DEFINE_REGINDEX_END()

#endif

