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

  /* Network Registration Check */
  while (TYPE1SC.canConnect() != 0) {
    DebugSerial.println("Network not Ready!!!");
    u8x8log.print("Network not Ready!!!\n");
    delay(2000);
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");
  u8x8log.print("TYPE1SC Module Ready!!!\n");
}

void loop() {
  /*** TYPE1SC Basic Test Code ***/
  /* SIM Card Check */
  if (!TYPE1SC.chkSIM()) {
    DebugSerial.println("SIM Card OK!!!");
    u8x8log.print("SIM Card OK!!!\n");
  }
  delay(1000);

  /* Get Phone Number */
  char szCIMI[32];
  if (TYPE1SC.getCIMI(szCIMI, sizeof(szCIMI)) == 0) {
    DebugSerial.print("IMSI : ");
    DebugSerial.println(szCIMI);
    u8x8log.print("IMSI : ");
    u8x8log.print(szCIMI);
    u8x8log.print("\n");
  }
  delay(1000);

  /* Get IMEI Number */
  char szIMEI[32];
  if (TYPE1SC.getIMEI(szIMEI, sizeof(szIMEI)) == 0) {
    DebugSerial.print("IMEI : ");
    DebugSerial.println(szIMEI);
    u8x8log.print("IMEI : ");
    u8x8log.print(szIMEI);
    u8x8log.print("\n");
  }
  delay(1000);

  /* Get ICCID Number */
  char szICCID[32];
  if (TYPE1SC.getICCID(szICCID, sizeof(szICCID)) == 0) {
    DebugSerial.print("ICCID : ");
    DebugSerial.println(szICCID);
    u8x8log.print("ICCID : ");
    u8x8log.print(szICCID);
    u8x8log.print("\n");
  }
  delay(1000);

  /* Get Fimrware version */
  char szCGMR[32];
  if (TYPE1SC.getCGMR(szCGMR, sizeof(szCGMR)) == 0) {
    DebugSerial.print("CGMR : ");
    DebugSerial.println(szCGMR);
    u8x8log.print("CGMR : ");
    u8x8log.print(szCGMR);
    u8x8log.print("\n");
  }
  delay(1000);

  /* Get Time (GMT, (+36/4) ==> Korea +9hour) */
  char szTime[32];
  if (TYPE1SC.getCCLK(szTime, sizeof(szTime)) == 0) {
    DebugSerial.print("Time : ");
    DebugSerial.println(szTime);
    u8x8log.print("Time : ");
    u8x8log.print(szTime);
    u8x8log.print("\n");
  }
  delay(1000);

  /* Get RSSI */
  int rssi;
  if (TYPE1SC.getRSSI(&rssi) == 0) {
    DebugSerial.print("RSSI : ");
    DebugSerial.println(rssi);
    u8x8log.print("RSSI : ");
    u8x8log.print(rssi);
    u8x8log.print("\n");
  }
  delay(1000);

  /* Get RSRP */
  int rsrp;
  if (TYPE1SC.getRSRP(&rsrp) == 0) {
    DebugSerial.print("RSRP : ");
    DebugSerial.println(rsrp);
    u8x8log.print("RSRP : ");
    u8x8log.print(rsrp);
    u8x8log.print("\n");
  }
  delay(1000);

  /* Get RSRQ */
  int rsrq;
  if (TYPE1SC.getRSRQ(&rsrq) == 0) {
    DebugSerial.print("RSRQ : ");
    DebugSerial.println(rsrq);
    u8x8log.print("RSRQ : ");
    u8x8log.print(rsrq);
    u8x8log.print("\n");
  }
  delay(1000);

  /* Get SINR */
  int sinr;
  if (TYPE1SC.getSINR(&sinr) == 0) {
    DebugSerial.print("SINR : ");
    DebugSerial.println(sinr);
    u8x8log.print("SINR : ");
    u8x8log.print(sinr);
    u8x8log.print("\n");
  }
  delay(1000);

  /* Get TX Power */
  char txPower[64];
  if (TYPE1SC.getTxPower(txPower, sizeof(txPower)) == 0) {
    DebugSerial.print("TX Power : ");
    DebugSerial.println(txPower);
    u8x8log.print("TX Power : ");
    u8x8log.print(txPower);
    u8x8log.print("\n");
  }
  delay(1000);
}
