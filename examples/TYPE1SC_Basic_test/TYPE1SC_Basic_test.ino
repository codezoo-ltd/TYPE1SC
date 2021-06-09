#include "TYPE1SC.h"

#define DebugSerial Serial
#define M1Serial Serial1
#define PWR_PIN 2
#define STAT_PIN 3

TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, STAT_PIN);

void setup() {
  // put your setup code here, to run once:
  M1Serial.begin(115200);
  DebugSerial.begin(115200);

  /* Power On Sequence */
  if (TYPE1SC.isPwrON()) {
    DebugSerial.println("TYPE1SC Power ON Status");
    if (TYPE1SC.pwrOFF()) {
      DebugSerial.println("TYPE1SC Power Off Error");
    } else {
      DebugSerial.println("TYPE1SC Power Off Success");
      DebugSerial.println("Module Power ON Sequence Start");
      if (TYPE1SC.pwrON()) {
        DebugSerial.println("TYPE1SC Power ON Error");
      } else
        DebugSerial.println("TYPE1SC Power ON Success");
    }
  } else {
    DebugSerial.println("TYPE1SC Power OFF Status");
    if (TYPE1SC.pwrON()) {
      DebugSerial.println("TYPE1SC Power ON Error");
    } else
      DebugSerial.println("TYPE1SC Power ON Success");
  }

  /* TYPE1SC Module Initialization */
  if (TYPE1SC.init()) {
    DebugSerial.println("TYPE1SC Module Error!!!");
  }

  /* TYPE1SC Module Power Saving Mode Disable */
  if (TYPE1SC.disablePSM()) {
    DebugSerial.println("TYPE1SC PSM Disable Error!!!");
  }

  /* Network Regsistraiton Check */
  while (TYPE1SC.canConnect() != 0) {
    DebugSerial.println("Network not Ready !!!");
    delay(2000);
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");

  /*** TYPE1SC Basic Test Code ***/
  /* Get Phone Number */
  char szCIMI[16];
  if (TYPE1SC.getCIMI(szCIMI, sizeof(szCIMI)) == 0) {
    DebugSerial.print("CIMI : ");
    DebugSerial.println(szCIMI);
  }

  /* Get IMEI */
  char szIMEI[16];
  if (TYPE1SC.getIMEI(szIMEI, sizeof(szIMEI)) == 0) {
    DebugSerial.print("IMEI : ");
    DebugSerial.println(szIMEI);
  }

  /* Get Fimrware version */
  char szCGMR[20];
  if (TYPE1SC.getCGMR(szCGMR, sizeof(szCGMR)) == 0) {
    DebugSerial.print("CGMR : ");
    DebugSerial.println(szCGMR);
  }

  /* Get ServingCell */
  char servingCell[10];
  if (TYPE1SC.getServingCell(servingCell) == 0) {
    DebugSerial.print("ServingCell : ");
    DebugSerial.println(servingCell);
  }

  /* Get RSRP */
  int rsrp;
  if (TYPE1SC.getRSRP(&rsrp) == 0) {
    DebugSerial.print("RSRP : ");
    DebugSerial.println(rsrp);
  }

  /* Get SINR */
  int sinr;
  if (TYPE1SC.getSINR(&sinr) == 0) {
    DebugSerial.print("SINR : ");
    DebugSerial.println(sinr);
  }

  /* Get SINR */
  int txPower;
  if (TYPE1SC.getTxPower(&txPower) == 0) {
    DebugSerial.print("TX Power : ");
    DebugSerial.println(txPower);
  }

  /* Get RSSI */
  int rssi;
  if (TYPE1SC.getRSSI(&rssi) == 0) {
    DebugSerial.print("RSSI : ");
    DebugSerial.println(rssi);
  }

  /* Get Time (GMT, (+36/4) ==> Korea +9hour) */
  char szTime[32];
  if (TYPE1SC.getCCLK(szTime, sizeof(szTime)) == 0) {
    DebugSerial.print("Time : ");
    DebugSerial.println(szTime);
  }

  /* Activate/Deactivate PDP Context */
  char szPDP[32];
  if (TYPE1SC.isActPDP(szPDP, sizeof(szPDP))) {
    if (TYPE1SC.actPDP() == 0) {
      if (TYPE1SC.isActPDP(szPDP, sizeof(szPDP)) == 0) {
        DebugSerial.print("Activation PDP info : ");
        /* 1(ContextID), 1(Activated), 1(IPV4), IP Addr(xxx.xxx.xxx.xxx) */
        DebugSerial.println(szPDP);
      }
    }
  }

  if (TYPE1SC.deActPDP() == 0) {
    DebugSerial.println("Deactivation PDP Context");
  }
}

void loop() { delay(100); }
