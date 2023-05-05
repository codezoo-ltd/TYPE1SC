#include "TYPE1SC.h"
#include "aws_credentials.h"
#include <Arduino.h>
#include <U8x8lib.h>

#define DebugSerial Serial
#define M1Serial Serial2

#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19
#define EXT_ANT 4

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);

#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
U8X8LOG u8x8log;

/* EXT_ANT_ON 0 : Use an internal antenna.
 * EXT_ANT_ON 1 : Use an external antenna.
 */
#define EXT_ANT_ON 0

void extAntenna(){
	if (EXT_ANT_ON) 
	{   
		pinMode(EXT_ANT, OUTPUT);
		digitalWrite(EXT_ANT, HIGH);
		delay(100);
	}   
}

void setup() {
	u8x8.begin();
	u8x8.setFont(u8x8_font_chroma48medium8_r);

	u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
	u8x8log.setRedrawMode(1);    // 0: Update screen with newline, 1: Update screen for every char

	// put your setup code here, to run once:
	M1Serial.begin(115200);
	DebugSerial.begin(115200);

	DebugSerial.println("TYPE1SC Module Start!!!");
	u8x8log.print("TYPE1SC Module Start!!!\n");

	extAntenna();	

	/* TYPE1SC Module Initialization */
	if (TYPE1SC.init()) {
		DebugSerial.println("TYPE1SC Module Error!!!");
		u8x8log.print("TYPE1SC Module Error!!!\n");
	}

	/* Network Regsistraiton Check */
	while (TYPE1SC.canConnect() != 0) {
		DebugSerial.println("Network not Ready!!!");
		u8x8log.print("Network not Ready!!!\n");
		delay(2000);
	}

	DebugSerial.println("TYPE1SC Module Ready!!!");
	u8x8log.print("TYPE1SC Module Ready!!!\n");

	/* DELETE Certification Profile 1-255 */
	int delProfile = 9;
	if (TYPE1SC.delCert(delProfile) == 0) {
		DebugSerial.println("Delete Certification Profile!!!");
		u8x8log.print("Delete Certification Profile!!!\n");
	}
	delay(2000);

	/* Write root CA, Don't edit the file name */
	if (TYPE1SC.writeKEY("rootCA.pem", 0, rootCA) == 0) {
		DebugSerial.println("Root CA Write!!!");
		u8x8log.print("Root CA Write!!!\n");
	}
	delay(5000);

	/* Write client CA, Don't edit the file name */
	if (TYPE1SC.writeKEY("cert.pem.crt", 0, clientCrt) == 0) {
		DebugSerial.println("Client CA Write!!!");
		u8x8log.print("Client CA Write!!!\n");
	}
	delay(5000);

	/* Write client KEY, Don't edit the file name */
	if (TYPE1SC.writeKEY("private.pem.key", 1, clientKey) == 0) {
		DebugSerial.println("Client KEY Write!!!");
		u8x8log.print("Client KEY Write!!!\n");
	}
	delay(5000);

	/* ADD Certification Profile 1-255 */
	int addProfile = 9;
	if (TYPE1SC.addCert(addProfile) == 0) {
		DebugSerial.println("ADD Certification Profile!!!");
		u8x8log.print("ADD Certification Profile!!!\n");
	}
}

void loop() { delay(1000); }
