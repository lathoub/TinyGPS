#include <SoftwareSerial.h>
#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
It requires the use of SoftwareSerial, and assumes that you have a
9600-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

SoftwareSerial ss(8, 9);
GPS_CREATE_INSTANCE(ss, gps);

void OnGPRMC(void* sender, EventArgs* e)
{
	dump(sender);
}

void setup()
{
	Serial.begin(115200);
	gps.begin();

	gps.GPRMC += OnGPRMC;

	Serial.print("Simple TinyGPS library v. "); Serial.println(TinyGPS<SoftwareSerial>::library_version());
	Serial.println("by Mikal Hart");
	Serial.println();
}

static void dump(void* sender)
{
	TinyGPS<SoftwareSerial>* uu = (TinyGPS<SoftwareSerial>*)sender;

	float flat, flon;
	unsigned long age;
	uu->f_get_position(&flat, &flon, &age);

	Serial.print("LAT=");
	Serial.print(flat == TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
	Serial.print(" LON=");
	Serial.print(flon == TinyGPS<SoftwareSerial>::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
	Serial.print(" SAT=");
	Serial.print(uu->satellites() == TinyGPS<SoftwareSerial>::GPS_INVALID_SATELLITES ? 0 : uu->satellites());
	Serial.print(" PREC=");
	Serial.print(uu->hdop() == TinyGPS<SoftwareSerial>::GPS_INVALID_HDOP ? 0 : uu->hdop());

	unsigned long chars;
	unsigned short sentences, failed;

	uu->stats(&chars, &sentences, &failed);
	Serial.print(" CHARS=");
	Serial.print(chars);
	Serial.print(" SENTENCES=");
	Serial.print(sentences);
	Serial.print(" CSUM ERR=");
	Serial.println(failed);
	if (chars == 0)
		Serial.println("** No characters received from GPS: check wiring **");
}

void loop()
{
	gps.evaluate();
}