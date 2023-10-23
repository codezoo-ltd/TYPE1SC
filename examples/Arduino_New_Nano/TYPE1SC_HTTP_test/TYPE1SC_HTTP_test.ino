#include "TYPE1SC.h"

#define DebugSerial Serial
#define M1Serial Serial1

#define DHTPIN A0
#include "DHT.h" /* https://github.com/markruys/arduino-DHT */
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

TYPE1SC TYPE1SC(M1Serial, DebugSerial);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  /* DHT22 Sensor Initialization */
  dht.begin();
  delay(2000);

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

  /* Enter a DNS address to get an IP address */
  char IPAddr[32];

  while (1) {

    if (TYPE1SC.getIPAddr("api.thingspeak.com", IPAddr, sizeof(IPAddr)) == 0) {
      DebugSerial.print("IP Address : ");
      DebugSerial.println(IPAddr);
      break;
    } else {
      DebugSerial.println("IP Address Error!!!");
    }
    delay(2000);
  }

  int _PORT = 80;
  char sckInfo[128];
  char recvBuffer[700];
  int recvSize;

  String WApiKey = "****************"; // Thing Speak Write API Key 16Character
  float temp = 0.0;
  float humi = 0.0;
  String fieldTemp = "field1"; // Air temperature
  String fieldHumi = "field2"; // Air humidity

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

  /* 1 :TCP Socket Create ( 0:UDP, 1:TCP ) */
  if (TYPE1SC.socketCreate(1, IPAddr, _PORT) == 0)
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
  String data = "GET /update";
  data += "?api_key=" + WApiKey + "&" + fieldTemp + "=" + String(temp) + "&" +
          fieldHumi + "=" + String(humi);
  data += " HTTP/1.1\r\n";
  data += "Host: api.thingspeak.com\r\n";
  data += "Connection: close\r\n\r\n";

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
}

void loop() { delay(1000); }
