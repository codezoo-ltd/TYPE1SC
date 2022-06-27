#include "TYPE1SC.h"
#include <UnixTime.h> //https://github.com/GyverLibs/UnixTime
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
UnixTime stamp(9); // Seoul GMT + 09

TYPE1SC TYPE1SC(M1Serial, DebugSerial);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  /* DHT22 Sensor Initialization */
  dht.begin();
  delay(2000);
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

  /* Device Data EndPoint Address AWS IoT > Settings > Device data endpoint >
   * Copy&Paste */
  char _IP[] = "*****.amazonaws.com";

  char _NodeID[] = "MCU_AI_Things";
  char _Topic[] = "iot/topic";
  char _message[512];
  int tlsProfile = 9;
  int conn_timeout = 1200;
  float t = 0.0; // Stores temperature value
  float h = 0.0; // Stores humidity value
  char temp[8];
  char humi[8];

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

  // Make Unix TimeStamp
  char szTime[32];
  uint32_t _year, _month, _day, _hour, _minute, _second, _tmp;

  if (TYPE1SC.getCCLK(szTime, sizeof(szTime)) == 0) {
    sscanf(szTime, "\"%d/%d/%d,%d:%d:%d+%d\"", &_year, &_month, &_day, &_hour,
           &_minute, &_second, &_tmp);
  }

  // Set Date Time
  _year += 2000;
  stamp.setDateTime(_year, _month, _day, _hour, _minute, _second);

  // Get Unix Time
  uint32_t unix = stamp.getUnix();

  memset(_message, 0x0, sizeof(_message));
  sprintf(_message,
          "{\\\"timeInSeconds\\\":\\\"%lu\\\",\\\"Temperature\\\":\\\"%s\\\","
          "\\\"Humidity\\\":\\\"%s\\\"}",
          unix, temp, humi);

  /* 6 : Publish data to broker */
  if (TYPE1SC.AWSIOT_Publish(_Topic, _message) == 0)
    DebugSerial.println("6.Publish data to broker");

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
}

void loop() { delay(1000); }
