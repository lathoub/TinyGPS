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

#define KTS 1 // Knots
#define MPH 2 // miles per hour
#define KPH 3 // km per hour
#define MPS 4 // meters per second
#define MPM 5 // miles per meter

// -----------------------------------------------------------------------------

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) << 5) | term_number)

// -----------------------------------------------------------------------------

#define _GPS_NO_STATS

// -----------------------------------------------------------------------------

#define GPS_CREATE_INSTANCE(SerialPort, Name) \
    TinyGPS<typeof(SerialPort)> Name((typeof(SerialPort)&)SerialPort);

#if defined(ARDUINO_SAM_DUE) || defined(USBCON)
// Leonardo, Due and other USB boards use Serial1 by default.
#define GPS_CREATE_DEFAULT_INSTANCE()                                      \
        GPS_CREATE_INSTANCE(Serial1, gps);
#else
/*! \brief Create an instance of the library with default name, serial port
and settings, for compatibility with sketches written with pre-v4.2 MIDI Lib,
or if you don't bother using custom names, serial port or settings.
*/
#define GPS_CREATE_DEFAULT_INSTANCE()                                      \
        GPS_CREATE_INSTANCE(Serial,  gps);
#endif