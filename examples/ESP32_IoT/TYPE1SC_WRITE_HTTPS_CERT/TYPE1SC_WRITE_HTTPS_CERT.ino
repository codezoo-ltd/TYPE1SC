#include "TYPE1SC.h"

#include "http_credentials.h"
#include <Arduino.h>
#include <U8x8lib.h>

#define DebugSerial Serial
#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19
#define EXT_ANT 4

#define PROFILE_ID 2

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);
HardwareSerial M1Serial(2); // use ESP32 UART2
TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);

#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
U8X8LOG u8x8log;

/* EXT_ANT_ON 0 : Use an internal antenna.
 * EXT_ANT_ON 1 : Use an external antenna.
 */
#define EXT_ANT_ON 0

void extAntenna() {
  if (EXT_ANT_ON) {
    pinMode(EXT_ANT, OUTPUT);
    digitalWrite(EXT_ANT, HIGH);
    delay(100);
  }
}

void setup() {
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8x8log.setRedrawMode(
      1); // 0: Update screen with newline, 1: Update screen for every char
 
  /* Serial2 Initialization */
  M1Serial.begin(115200, SERIAL_8N1, 16, 17); // RXD2 : 16, TXD2 : 17
  DebugSerial.begin(115200);

  DebugSerial.println("TYPE1SC Module Start!!!");
  u8x8log.print("TYPE1SC Module Start!!!\n");

  extAntenna();

  /* Board Reset */
  TYPE1SC.reset();
  delay(2000);

  /* TYPE1SC Module Initialization */
  if (TYPE1SC.init()) {
    DebugSerial.println("TYPE1SC Module Error!!!");
    u8x8log.print("TYPE1SC Module Error!!!\n");
  }

  /* Network Regsistraiton Check */
  while (TYPE1SC.canConnect() != 0) {
    DebugSerial.println("Network not Ready !!!");
    u8x8log.print("Network not Ready!!!\n");
    delay(2000);
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");
  u8x8log.print("TYPE1SC Module Ready!!!\n");

  /* DELETE Certification Profile 1-255 */
  int delProfile = PROFILE_ID;
  if (TYPE1SC.delCert(delProfile) == 0) {
    DebugSerial.println("Delete Certification in Profile..");
    u8x8log.print("Delete Certification Profile!!!\n");
  }
  delay(2000);

  /* Write server CA, Don't edit the file name */
  if (TYPE1SC.writeKEY("server.crt", 0, serverCrt) == 0) {
    DebugSerial.println("Server CA Write!!!");
    u8x8log.print("Server CA Write!!!\n");
  }
  delay(5000);

  /* ADD Certification Profile 1-255 */
  int addProfile = PROFILE_ID;
  if (TYPE1SC.addHTTPCert(addProfile) == 0) {
    DebugSerial.println("ADD Server Certification in Profile..");
    u8x8log.print("ADD Certification Profile!!!\n");
  }
}

void loop() { delay(1000); }
