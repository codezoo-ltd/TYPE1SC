#include "TYPE1SC.h"
#if !defined(__AVR_ATmega4809__)
#include <avr/dtostrf.h>
#endif

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

  char _HTTP_ADDR[] = "https://httpbin.org";
  char _HTTP_GET_ADDR[] = "https://httpbin.org/get";
  int profile_id = 1;
  int nProfile = 3;
  int readSize = 0;
  char readData[1024];

  /* 1 : Enable HTTP events */
  if (TYPE1SC.setHTTP_EV(1) == 0)
    DebugSerial.println("1.Enable HTTP events");

  /* 2 : Configure http parameters (profile ID, http Address) */
  if (TYPE1SC.setHTTP_NODES(profile_id, _HTTP_ADDR) == 0)
    DebugSerial.println("2.Configure http parameter:profile ID, http Address");

  /* 3 : Configure HTTP TLS (If you connect https Server then Must be set!!) */
  if (TYPE1SC.setHTTP_TLS(profile_id, nProfile) == 0)
    DebugSerial.println("3.Configure HTTP TLS");

  /* 4 : HTTP GET Command to Server */
  if (TYPE1SC.HTTP_GET(profile_id, _HTTP_GET_ADDR, &readSize) == 0) {
    DebugSerial.println("4.HTTP GET Command to Server");
    DebugSerial.print("http readSize : ");
    DebugSerial.println(readSize);
  }

  /* 5 : HTTP Read Data from Server */
  if (TYPE1SC.HTTP_READ(profile_id, readSize, readData, sizeof(readData)) ==
      0) {
    DebugSerial.println("5.HTTP Read Data from Server");
    for (int i = 0; i < readSize; i++) {
      DebugSerial.print(readData[i]);
    }
  }
}

void loop() { delay(1000); }
