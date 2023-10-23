#include "TYPE1SC.h"

#include "http_credentials.h"

#define DebugSerial Serial
#define M1Serial Serial1

#define PROFILE_ID 2

TYPE1SC TYPE1SC(M1Serial, DebugSerial);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

#if defined(ARDUINO_NANO_ESP32)
  M1Serial.begin(115200, SERIAL_8N1, D0, D1); 
#else
  M1Serial.begin(115200);
#endif  
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

  /* DELETE Certification Profile 1-255 */
  int delProfile = PROFILE_ID;
  if (TYPE1SC.delCert(delProfile) == 0) {
    DebugSerial.println("Delete Certification in Profile..");
  }
  delay(2000);

  /* Write server CA, Don't edit the file name */
  if (TYPE1SC.writeKEY("server.crt", 0, serverCrt) == 0) {
    DebugSerial.println("Server CA Write!!!");
  }
  delay(5000);

  /* ADD Certification Profile 1-255 */
  int addProfile = PROFILE_ID;
  if (TYPE1SC.addHTTPCert(addProfile) == 0) {
    DebugSerial.println("ADD Server Certification in Profile..");
  }
}

void loop() { delay(1000); }
