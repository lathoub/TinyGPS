#pragma once

#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef uint8_t byte;
#endif

#include <stdlib.h>

#include "Events.h"
#include "nmea_Defs.h"

// -----------------------------------------------------------------------------

#define _GPS_VERSION 14 // software version of this library
#define _GPS_MPH_PER_KNOT 1.15077945
#define _GPS_MPS_PER_KNOT 0.51444444
#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MILES_PER_METER 0.00062137112
#define _GPS_KM_PER_METER 0.001

// -----------------------------------------------------------------------------

//#define _GPS_NO_STATS

#define GPS_CREATE_INSTANCE(SerialPort, Name) \
    TinyGPS<typeof(SerialPort)> Name((typeof(SerialPort)&)SerialPort);
