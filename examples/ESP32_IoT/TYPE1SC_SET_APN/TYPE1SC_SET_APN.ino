#include "TYPE1SC.h"
#include <Arduino.h>
#include <U8x8lib.h>

#define DebugSerial Serial
#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19
#define EXT_ANT 4

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

  // put your setup code here, to run once:
  /* Serial2 Initialization */
  M1Serial.begin(115200, SERIAL_8N1, 16, 17); // RXD2 : 16, TXD2 : 17
  DebugSerial.begin(115200);

  DebugSerial.println("TYPE1SC Module Start!!!");
  u8x8log.print("TYPE1SC Module Start!!!\n");

  extAntenna();

  /* TYPE1SC Module Initialization */
  if (TYPE1SC.init()) {
    DebugSerial.println("TYPE1SC Module Error!!!");
    u8x8log.print("TYPE1SC Module Error!!!\n");
  }

  /* Network Disable */
  if (TYPE1SC.setCFUN(0) == 0) {
    DebugSerial.println("TYPE1SC Network Disable!!!");
    u8x8log.print("TYPE1SC Network Disable!!!\n");
  }

  delay(1000);

  char *apnAddr = "simplio.apn"; /* Vodafone Global IoT SIM APN */

  if (TYPE1SC.setAPN(apnAddr) == 0) {
    DebugSerial.println("TYPE1SC Set APN Address!!!");
    u8x8log.print("TYPE1SC Set APN Address!!!\n");
  }

  /* Board Reset */
  TYPE1SC.reset();
  delay(2000);

  /* TYPE1SC Module Initialization */
  if (TYPE1SC.init()) {
    DebugSerial.println("TYPE1SC Module Error!!!");
    u8x8log.print("TYPE1SC Module Error!!!\n");
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");
  u8x8log.print("TYPE1SC Module Ready!!\n");

  char apn[128];
  if (TYPE1SC.getAPN(apn, sizeof(apn)) == 0) {
    DebugSerial.print("GET APN Address: ");
    DebugSerial.println(apn);
    u8x8log.print("GET APN Address: ");
    u8x8log.print(apn);
    u8x8log.print("\n");
  }

  DebugSerial.println("TYPE1SC APN Setup Complete!!!");
  u8x8log.print("TYPE1SC APN Setup Complete!!!\n");
}

void loop() { delay(1000); }
