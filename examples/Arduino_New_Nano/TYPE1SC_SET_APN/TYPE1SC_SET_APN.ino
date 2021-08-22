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
  /* TYPE1SC Module Initialization */
  if (TYPE1SC.init()) {
    DebugSerial.println("TYPE1SC Module Error!!!");
  }

  /* Board Reset */
  TYPE1SC.reset();

  delay(2000);

  /* Network Disable */
  if (TYPE1SC.setCFUN(0) == 0) {
    DebugSerial.println("TYPE1SC Network Disable!!!");
  }

  delay(1000);

  char *apnAddr = "internet.lte.cxn";

  if (TYPE1SC.setAPN(apnAddr) == 0) {
    DebugSerial.println("TYPE1SC Set APN Address !!!");
  }

  /* Board Reset */
  TYPE1SC.reset();

  delay(2000);

  /* TYPE1SC Module Initialization */
  if (TYPE1SC.init()) {
    DebugSerial.println("TYPE1SC Module Error!!!");
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");

  char apn[128];
  if (TYPE1SC.getAPN(apn, sizeof(apn)) == 0) {
    DebugSerial.print("GET APN Address: ");
    DebugSerial.println(apn);
  }
}

void loop() { delay(1000); }
