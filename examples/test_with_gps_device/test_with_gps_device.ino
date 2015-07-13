//#include <HardwareSerial.h>
#include <TinyGPS.h>

#include <avr/power.h>
#include <avr/sleep.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
It requires the use of HardwareSerial, and assumes that you have a
4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

//HardwareSerial ss(8, 9);
//GPS_CREATE_INSTANCE(ss, gps);

GPS_CREATE_DEFAULT_INSTANCE();

void setup()
{
  Serial.begin(115200);
  while (!Serial) {} // wait for serial port to connect. Needed for Leonardo only

  Serial.print("Testing TinyGPS library v. "); Serial.println(TinyGPS<HardwareSerial>::library_version());
  Serial.println("by Mikal Hart");
  Serial.println();
  Serial.println("Sats HDOP Latitude  Longitude  Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum");
  Serial.println("          (deg)     (deg)      Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail");
  Serial.println("-------------------------------------------------------------------------------------------------------------------------------------");

  gps.begin();

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  gps.sendCommand(PGCMD_ANTENNA);

  gps.GPRMC += OnGPRMC;
  gps.GPGGA += OnGPGGA;
}

void sleepNow()
{
  /* Now is the time to set the sleep mode. In the Atmega8 datasheet
   * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
   * there is a list of sleep modes which explains which clocks and
   * wake up sources are available in which sleep modus.
   *
   * In the avr/sleep.h file, the call names of these sleep modus are to be found:
   *
   * The 5 different modes are:
   *     SLEEP_MODE_IDLE         -the least power savings
   *     SLEEP_MODE_ADC
   *     SLEEP_MODE_PWR_SAVE
   *     SLEEP_MODE_STANDBY
   *     SLEEP_MODE_PWR_DOWN     -the most power savings
   *
   *  the power reduction management <avr/power.h>  is described in
   *  http://www.nongnu.org/avr-libc/user-manual/group__avr__power.html
   */

  set_sleep_mode(SLEEP_MODE_PWR_SAVE);   // sleep mode is set here

  sleep_enable();          // enables the sleep bit in the mcucr register
  // so sleep is possible. just a safety pin

  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

  sleep_mode();            // here the device is actually put to sleep!!

  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
  sleep_disable();         // first thing after waking from sleep:
  // disable sleep...

  power_all_enable();
}

void loop()
{
  gps.evaluate();

  //  delay(100);     // this delay is needed, the sleep
  // function will provoke a Serial error otherwise!!
 // sleepNow();     // sleep function called here
}






void OnGPRMC(void* sender, EventArgs* e)
{
  dump(sender);
}

void OnGPGGA(void* sender, EventArgs* e)
{
  dump(sender);
}



static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int) val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  //smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  //smartdelay(0);
}

static void print_date(TinyGPS<HardwareSerial>* gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps->crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS<HardwareSerial>::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
            month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS<HardwareSerial>::GPS_INVALID_AGE, 5);
  //smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    Serial.print(i < slen ? str[i] : ' ');
  //smartdelay(0);
}

static void dump(void* sender)
{
  TinyGPS<HardwareSerial>* uu = (TinyGPS<HardwareSerial>*)sender;

  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  print_int(gps.satellites(), TinyGPS<HardwareSerial>::GPS_INVALID_SATELLITES, 5);
  print_int(gps.hdop(), TinyGPS<HardwareSerial>::GPS_INVALID_HDOP, 5);
  gps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS<HardwareSerial>::GPS_INVALID_F_ANGLE, 10, 6);
  print_float(flon, TinyGPS<HardwareSerial>::GPS_INVALID_F_ANGLE, 11, 6);
  print_int(age, TinyGPS<HardwareSerial>::GPS_INVALID_AGE, 5);
  print_date(uu);
  print_float(gps.f_altitude(), TinyGPS<HardwareSerial>::GPS_INVALID_F_ALTITUDE, 7, 2);
  print_float(gps.f_course(), TinyGPS<HardwareSerial>::GPS_INVALID_F_ANGLE, 7, 2);
  print_float(gps.f_speed(KPH), TinyGPS<HardwareSerial>::GPS_INVALID_F_SPEED, 6, 2);
  print_str(gps.f_course() == TinyGPS<HardwareSerial>::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS<HardwareSerial>::cardinal(gps.f_course()), 6);
  print_int(flat == TinyGPS<HardwareSerial>::GPS_INVALID_F_ANGLE ? 0xFFFFFFFF : (unsigned long) TinyGPS<HardwareSerial>::distance_between(flat, flon, LONDON_LAT, LONDON_LON) / 1000, 0xFFFFFFFF, 9);
  print_float(flat == TinyGPS<HardwareSerial>::GPS_INVALID_F_ANGLE ? TinyGPS<HardwareSerial>::GPS_INVALID_F_ANGLE : TinyGPS<HardwareSerial>::course_to(flat, flon, LONDON_LAT, LONDON_LON), TinyGPS<HardwareSerial>::GPS_INVALID_F_ANGLE, 7, 2);
  print_str(flat == TinyGPS<HardwareSerial>::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS<HardwareSerial>::cardinal(TinyGPS<HardwareSerial>::course_to(flat, flon, LONDON_LAT, LONDON_LON)), 6);

  //	gps.stats(&chars, &sentences, &failed);
  //	print_int(chars, 0xFFFFFFFF, 6);
  //	print_int(sentences, 0xFFFFFFFF, 10);
  //	print_int(failed, 0xFFFFFFFF, 9);

  Serial.println();
}
