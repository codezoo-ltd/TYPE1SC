#include "TYPE1SC.h"

#define DebugSerial Serial
#define M1Serial Serial1 // Arduino
//#define M1Serial Serial2 //ESP32

#define PWR_PIN 25
#define STAT_PIN 26
//#define PWR_PIN 2
//#define STAT_PIN 3

#define DHTPIN A0
// AM2302(DHT22) Temperature & Humidity Sensor
#include "DHT.h" /* https://github.com/markruys/arduino-DHT */
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

/*
  #define DebugSerial Serial
  #define M1Serial
  Serial2 // 16번핀 -- CAT.M1 TXD
          // 17번핀 -- CAT.M1 RXD
  #define PWR_PIN A19  // 25번핀 -- CAT.M1 STATUS
  #define STAT_PIN A18 // 26번핀 -- CAT.M1 POWER_KEY
*/

TYPE1SC TYPE1SC(M1Serial, DebugSerial);

void setup() {
  dht.begin();
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  delay(2000);
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
  /* Network Regsistraiton Check */
  while (TYPE1SC.canConnect() != 0) {
    DebugSerial.println("Network not Ready !!!");
    delay(2000);
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");
  /*** TYPE1SC Basic Test Code ***/
}

void loop() {
#if 1
  /*
    char *apn = "internet.lte.cxn";
    if (TYPE1SC.setAPN(apn) == 0) {
      DebugSerial.print("APN Set : ");
      DebugSerial.println(apn);
    }
    delay(1000);
  */
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
#else
#if 0
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  /* Enter a DNS address to get an IP address */
  char IPAddr[32];
  if (TYPE1SC.getIPAddr("echo.mbedcloudtesting.com", IPAddr, sizeof(IPAddr)) == 0) {
    DebugSerial.print("IP Address : ");
    DebugSerial.println(IPAddr);
  }
  delay(1000);

  char _IP[] = "52.215.34.155";
  int _PORT = 7;
  char sckInfo[128];

  /* 1 :TCP Socket Create */
  if (TYPE1SC.socketCreate(1, _IP, _PORT) == 0)
    DebugSerial.println("TCP Socket Create!!!");

INFO:

  /* 2 :TCP Socket Activation */
  if (TYPE1SC.socketActivate() == 0)
    DebugSerial.println("TCP Socket Activation!!!");

  if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
    DebugSerial.print("Socket Info : ");
    DebugSerial.println(sckInfo);

    if (strcmp(sckInfo, "ACTIVATED")) {
      delay(3000);
      goto INFO;
    }
  }

  /* 3 :TCP Socket Send Data */
  char sendBuffer[] = "Hello CodeZoo!!!";
  char recvBuffer[32];
  int  recvSize;

  if (TYPE1SC.socketSend(sendBuffer) == 0) {
    DebugSerial.print("[Send] >>  ");
    DebugSerial.println(sendBuffer);
  }
  else
    DebugSerial.println("Send Fail!!!");

  /* 4 :TCP Socket Recv Data */
  if (TYPE1SC.socketRecv(recvBuffer, sizeof(recvBuffer), &recvSize) == 0) {
    DebugSerial.print("[Recv] >>  ");
    DebugSerial.println(recvBuffer);
    DebugSerial.print("[RecvSize] >>  ");
    DebugSerial.println(recvSize);
  } else {
    DebugSerial.println("Recv Fail!!!");
  }

  /* 5 :TCP Socket DeActivation */
  if (TYPE1SC.socketDeActivate() == 0)
    DebugSerial.println("TCP Socket DeActivation!!!");

  if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
    DebugSerial.print("Socket Info : ");
    DebugSerial.println(sckInfo);
  }

  /* 5 :TCP Socket DeActivation */
  if (TYPE1SC.socketClose() == 0)
    DebugSerial.println("TCP Socket Close!!!");

  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
#else
  digitalWrite(LED_BUILTIN,
               HIGH); // turn the LED on (HIGH is the voltage level)

  String WApiKey = "OWS3SXKWM35SKANY"; // Thing Speak Write API Key 16Character
  float temp = 0.0;
  float humi = 0.0;
  String fieldTemp = "field1"; // Air temperature
  String fieldHumi = "field2"; // Air humidity

  char _IP[] = "3.224.210.136";
  int _PORT = 80;
  char sckInfo[128];
  char recvBuffer[700];
  int recvSize;

  /*Get Temperature & Humidity */
  while (1) {
    /* Get DHT22 Sensor */
    temp = dht.readTemperature();
    humi = dht.readHumidity();
    if (String(temp) != "nan" && String(humi) != "nan")
      break;
    else {
      DebugSerial.println("case nan ...");
      delay(1000);
    }
  }
  /* 1 :TCP Socket Create */
  if (TYPE1SC.socketCreate(1, _IP, _PORT) == 0)
    DebugSerial.println("TCP Socket Create!!!");

INFO:

  /* 2 :TCP Socket Activation */
  if (TYPE1SC.socketActivate() == 0)
    DebugSerial.println("TCP Socket Activation!!!");

  if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
    DebugSerial.print("Socket Info : ");
    DebugSerial.println(sckInfo);

    if (strcmp(sckInfo, "ACTIVATED")) {
      delay(3000);
      goto INFO;
    }
  }

  String data = "GET /update";
  data += "?api_key=" + WApiKey + "&" + fieldTemp + "=" + String(temp) + "&" +
          fieldHumi + "=" + String(humi);
  data += " HTTP/1.1\r\n";
  data += "Host: api.thingspeak.com\r\n";
  data += "Connection: close\r\n\r\n";

  /* 3 :TCP Socket Send Data */
  if (TYPE1SC.socketSend(data.c_str()) == 0) {
    DebugSerial.print("[HTTP Send] >>  ");
    DebugSerial.println(data);
  } else
    DebugSerial.println("Send Fail!!!");

  /* 4 :TCP Socket Recv Data */
  if (TYPE1SC.socketRecv(recvBuffer, sizeof(recvBuffer), &recvSize) == 0) {
    DebugSerial.print("[Recv] >>  ");
    DebugSerial.println(recvBuffer);
    DebugSerial.print("[RecvSize] >>  ");
    DebugSerial.println(recvSize);
  } else {
    DebugSerial.println("Recv Fail!!!");
  }

  /* 5 :TCP Socket DeActivation */
  if (TYPE1SC.socketDeActivate() == 0)
    DebugSerial.println("TCP Socket DeActivation!!!");

  if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
    DebugSerial.print("Socket Info : ");
    DebugSerial.println(sckInfo);
  }

  /* 6 :TCP Socket DeActivation */
  if (TYPE1SC.socketClose() == 0)
    DebugSerial.println("TCP Socket Close!!!");
  digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW

#endif
  delay(60000);
#endif
}
