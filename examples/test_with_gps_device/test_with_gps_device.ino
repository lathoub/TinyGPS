#include <SoftwareSerial.h>

#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
It requires the use of SoftwareSerial, and assumes that you have a
4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

SoftwareSerial ss(8, 9);
GPS_CREATE_INSTANCE(ss, gps);

void setup()
{
	Serial.begin(115200);

	Serial.print("Testing TinyGPS library v. "); Serial.println(TinyGPS<SoftwareSerial>::library_version());
	Serial.println("by Mikal Hart");
	Serial.println();
	Serial.println("Sats HDOP Latitude  Longitude  Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum");
	Serial.println("          (deg)     (deg)      Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail");
	Serial.println("-------------------------------------------------------------------------------------------------------------------------------------");

	gps.begin();
	gps.GPRMC += OnGPRMC;
	gps.GPGGA += OnGPGGA;
}

void loop()
{
	gps.evaluate();
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
		for (int i = flen; i<len; ++i)
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
	for (int i = strlen(sz); i<len; ++i)
		sz[i] = ' ';
	if (len > 0)
		sz[len - 1] = ' ';
	Serial.print(sz);
	//smartdelay(0);
}

static void print_date(TinyGPS<SoftwareSerial>* gps)
{
	int year;
	byte month, day, hour, minute, second, hundredths;
	unsigned long age;
	gps->crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
	if (age == TinyGPS<SoftwareSerial>::GPS_INVALID_AGE)
		Serial.print("********** ******** ");
	else
	{
		char sz[32];
		sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
			month, day, year, hour, minute, second);
		Serial.print(sz);
	}
	print_int(age, TinyGPS<SoftwareSerial>::GPS_INVALID_AGE, 5);
	//smartdelay(0);
}

static void print_str(const char *str, int len)
{
	int slen = strlen(str);
	for (int i = 0; i<len; ++i)
		Serial.print(i<slen ? str[i] : ' ');
	//smartdelay(0);
}

static void dump(void* sender)
{
	TinyGPS<SoftwareSerial>* uu = (TinyGPS<SoftwareSerial>*)sender;

	float flat, flon;
	unsigned long age, date, time, chars = 0;
	unsigned short sentences = 0, failed = 0;
	static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

	print_int(gps.satellites(), TinyGPS<SoftwareSerial>::GPS_INVALID_SATELLITES, 5);
	print_int(gps.hdop(), TinyGPS<SoftwareSerial>::GPS_INVALID_HDOP, 5);
	gps.f_get_position(&flat, &flon, &age);
	print_float(flat, TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE, 10, 6);
	print_float(flon, TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE, 11, 6);
	print_int(age, TinyGPS<SoftwareSerial>::GPS_INVALID_AGE, 5);
	print_date(uu);
	print_float(gps.f_altitude(), TinyGPS<SoftwareSerial>::GPS_INVALID_F_ALTITUDE, 7, 2);
	print_float(gps.f_course(), TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE, 7, 2);
	print_float(gps.f_speed(KPH), TinyGPS<SoftwareSerial>::GPS_INVALID_F_SPEED, 6, 2);
	print_str(gps.f_course() == TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS<SoftwareSerial>::cardinal(gps.f_course()), 6);
	print_int(flat == TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE ? 0xFFFFFFFF : (unsigned long) TinyGPS<SoftwareSerial>::distance_between(flat, flon, LONDON_LAT, LONDON_LON) / 1000, 0xFFFFFFFF, 9);
	print_float(flat == TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE ? TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE : TinyGPS<SoftwareSerial>::course_to(flat, flon, LONDON_LAT, LONDON_LON), TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE, 7, 2);
	print_str(flat == TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS<SoftwareSerial>::cardinal(TinyGPS<SoftwareSerial>::course_to(flat, flon, LONDON_LAT, LONDON_LON)), 6);

	gps.stats(&chars, &sentences, &failed);
	print_int(chars, 0xFFFFFFFF, 6);
	print_int(sentences, 0xFFFFFFFF, 10);
	print_int(failed, 0xFFFFFFFF, 9);
	Serial.println();
}