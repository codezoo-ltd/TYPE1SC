#include "TYPE1SC.h"

#define DebugSerial Serial
#define M1Serial Serial2 // ESP32
#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19

TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);

void setup() {

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

  /* Enter a DNS address to get an IP address */
  char IPAddr[32];

  while (1) {

    if (TYPE1SC.getIPAddr("echo.mbedcloudtesting.com", IPAddr,
                          sizeof(IPAddr)) == 0) {
      DebugSerial.print("IP Address : ");
      DebugSerial.println(IPAddr);
      break;
    } else {
      DebugSerial.println("IP Address Error!!!");
    }
    delay(2000);
  }

  int _PORT = 7;
  char sckInfo[128];

  /* 1 :UDP Socket Create ( 0:UDP, 1:TCP ) */
  if (TYPE1SC.socketCreate(0, IPAddr, _PORT) == 0)
    DebugSerial.println("UDP Socket Create!!!");

INFO:

  /* 2 :UDP Socket Activation */
  if (TYPE1SC.socketActivate() == 0)
    DebugSerial.println("UDP Socket Activation!!!");

  if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
    DebugSerial.print("Socket Info : ");
    DebugSerial.println(sckInfo);

    if (strcmp(sckInfo, "ACTIVATED")) {
      delay(3000);
      goto INFO;
    }
  }

  /* 3 :UDP Socket Send Data */
  char sendBuffer[] = "Hello CodeZoo!!!";
  char recvBuffer[32];
  int recvSize;

  if (TYPE1SC.socketSend(sendBuffer) == 0) {
    DebugSerial.print("[Send] >>  ");
    DebugSerial.println(sendBuffer);
  } else
    DebugSerial.println("Send Fail!!!");

  /* 4 :UDP Socket Recv Data */
  if (TYPE1SC.socketRecv(recvBuffer, sizeof(recvBuffer), &recvSize) == 0) {
    DebugSerial.print("[Recv] >>  ");
    DebugSerial.println(recvBuffer);
    DebugSerial.print("[RecvSize] >>  ");
    DebugSerial.println(recvSize);
  } else {
    DebugSerial.println("Recv Fail!!!");
  }

  /* 5 :UDP Socket DeActivation */
  if (TYPE1SC.socketDeActivate() == 0)
    DebugSerial.println("UDP Socket DeActivation!!!");

  if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
    DebugSerial.print("Socket Info : ");
    DebugSerial.println(sckInfo);
  }

  /* 6 :UDP Socket DeActivation */
  if (TYPE1SC.socketClose() == 0)
    DebugSerial.println("UDP Socket Close!!!");
}

void loop() { delay(1000); }
