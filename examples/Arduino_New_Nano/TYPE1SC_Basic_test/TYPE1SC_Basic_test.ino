#include "TYPE1SC.h"

#define DebugSerial Serial
#define M1Serial Serial1

TYPE1SC TYPE1SC(M1Serial, DebugSerial);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // put your setup code here, to run once:
  M1Serial.begin(115200);
  DebugSerial.begin(115200);

  DebugSerial.println("TYPE1SC Module Start!!!");
  /* Board Reset */
  TYPE1SC.reset();
  delay(2000);

  /* TYPE1SC Module Initialization */
  if (TYPE1SC.init()) {
    DebugSerial.println("TYPE1SC Module Error!!!");
  }

  /* Network Regsistraiton Check */
  while (TYPE1SC.canConnect() != 0) {
    DebugSerial.println("Network not Ready !!!");
    delay(2000);
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");
}

void loop() {
  /*** TYPE1SC Basic Test Code ***/
  /* Get Phone Number */
  char szCIMI[16];
  if (TYPE1SC.getCIMI(szCIMI, sizeof(szCIMI)) == 0) {
    DebugSerial.print("CIMI : ");
    DebugSerial.println(szCIMI);
  }
  delay(1000);

  char szIMEI[16];
  if (TYPE1SC.getIMEI(szIMEI, sizeof(szIMEI)) == 0) {
    DebugSerial.print("IMEI : ");
    DebugSerial.println(szIMEI);
  }
  delay(1000);

  /* Get Fimrware version */
  char szCGMR[20];
  if (TYPE1SC.getCGMR(szCGMR, sizeof(szCGMR)) == 0) {
    DebugSerial.print("CGMR : ");
    DebugSerial.println(szCGMR);
  }
  delay(1000);

  /* Get Time (GMT, (+36/4) ==> Korea +9hour) */
  char szTime[32];
  if (TYPE1SC.getCCLK(szTime, sizeof(szTime)) == 0) {
    DebugSerial.print("Time : ");
    DebugSerial.println(szTime);
  }
  delay(1000);

  /* Get RSSI */
  int rssi;
  if (TYPE1SC.getRSSI(&rssi) == 0) {
    DebugSerial.print("RSSI : ");
    DebugSerial.println(rssi);
  }
  delay(1000);

  /* Get RSRP */
  int rsrp;
  if (TYPE1SC.getRSRP(&rsrp) == 0) {
    DebugSerial.print("RSRP : ");
    DebugSerial.println(rsrp);
  }
  delay(1000);

  /* Get RSRQ */
  int rsrq;
  if (TYPE1SC.getRSRQ(&rsrq) == 0) {
    DebugSerial.print("RSRQ : ");
    DebugSerial.println(rsrq);
  }
  delay(1000);

  /* Get SINR */
  int sinr;
  if (TYPE1SC.getSINR(&sinr) == 0) {
    DebugSerial.print("SINR : ");
    DebugSerial.println(sinr);
  }
  delay(1000);

  /* Get Serving Cell ID */
  int cellID;
  if (TYPE1SC.getServingCell(&cellID) == 0) {
    DebugSerial.print("Cell ID : ");
    DebugSerial.println(cellID);
  }
  delay(1000);

  /* Get TX Power */
  char txPower[64];
  if (TYPE1SC.getTxPower(txPower, sizeof(txPower)) == 0) {
    DebugSerial.print("TX Power : ");
    DebugSerial.println(txPower);
  }
  delay(1000);
}
