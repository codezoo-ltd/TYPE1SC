#include "TYPE1SC.h"
#if !defined(__AVR_ATmega4809__)
#include <avr/dtostrf.h>
#endif

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
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  dht.begin();
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
}

void loop() {

  digitalWrite(LED_BUILTIN,
               HIGH); // turn the LED on (HIGH is the voltage level)

  /* Device Data EndPoint Address AWS IoT > Settings > Device data endpoint >
   * Copy&Paste */
  char _IP[] = "*****.amazonaws.com";
  char _NodeID[] = "Murata_Node01";
  char _Topic[] = "sdkTest/sub";
  char _message[64];
  int tlsProfile = 9;
  int conn_timeout = 1200;
  float t = 0.0; // Stores temperature value
  float h = 0.0; // Stores humidity value
  char temp[8];
  char humi[8];
  int cnt = 10; /* Data report Count */

  /* 1 : Configure AWS_IOT parameters (ID, Address, tlsProfile) */
  if (TYPE1SC.setAWSIOT_CONN(_NodeID, _IP, tlsProfile) == 0)
    DebugSerial.println(
        "1.Configure AWS_IOT parameter:ID, Address, tls Profile");

  /* 2 : Configure AWS_IOT parameters (Connection Timeout) */
  if (TYPE1SC.setAWSIOT_TIMEOUT(conn_timeout) == 0)
    DebugSerial.println("2.Configure AWS_IOT parameter:Timeout");

  /* 3 : Enable AWS_IOT events */
  if (TYPE1SC.setAWSIOT_EV(1) == 0)
    DebugSerial.println("3.Enable AWS_IOT events");

  /* 4 : Establish connection */
  if (TYPE1SC.AWSIOT_Connect() == 0) {
    DebugSerial.println("4.Establish connection");
  }

  /* 5 : Subscribe (register) to the topic on the endpoint */
  if (TYPE1SC.AWSIOT_SUBSCRIBE(_Topic) == 0)
    DebugSerial.println("5.Subscribe to the topic on the endpoint");

  /* 6 : Publish data to broker */
  while (cnt--) {
    /*Get Temperature & Humidity */
    while (1) {
      /* Get DHT22 Sensor */
      t = dht.readTemperature();
      h = dht.readHumidity();
      if (String(t) != "nan" && String(h) != "nan")
        break;
      else {
        DebugSerial.println("case nan ...");
        delay(1000);
      }
    }

    dtostrf(t, 4, 1, temp);
    dtostrf(h, 4, 1, humi);

    memset(_message, 0x0, sizeof(_message));
    sprintf(_message, "Temperature/%s, Humidity/%s", temp, humi);
    if (TYPE1SC.AWSIOT_Publish(_Topic, _message) == 0)
      DebugSerial.println("6.Publish data to broker");
  }

  /* 7 : UnSubscribe to the topic on the endpoint */
  if (TYPE1SC.AWSIOT_UnSUBSCRIBE(_Topic) == 0) {
    DebugSerial.println("7.UnSubscribe to the topic on the endpoint");
  }

  /* 8 : Disconnect AWS_IOT Service */
  if (TYPE1SC.AWSIOT_DisConnect() == 0)
    DebugSerial.println("8.Disconnect AWS_IOT Service");

  /* 9 : Disable AWS_IOT events */
  if (TYPE1SC.setAWSIOT_EV(0) == 0)
    DebugSerial.println("9.Disable AWS_IOT events");

  DebugSerial.println("/=========================================/");
  DebugSerial.println("");
  digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW

  delay(600000); /* 10 Minute */
}
