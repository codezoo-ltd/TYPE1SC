#include "TYPE1SC.h"

#define DebugSerial Serial
#define M1Serial Serial2 // ESP32
#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19

#define DHTPIN 33
#include "DHT.h" /* https://github.com/markruys/arduino-DHT */
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);

void setup() {
  dht.begin();
  // put your setup code here, to run once:
  M1Serial.begin(115200);
  DebugSerial.begin(115200);

  DebugSerial.println("TYPE1SC Module Start!!!");
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

  char _IP[] = "broker.hivemq.com";
  char _NodeID[] = "Cellular_node";
  char _Topic[] = "type1sc/0/test";
  char _message[64];
  uint32_t conn_timeout = 1200;
  float t = 0.0;
  float h = 0.0;
  char temp[8]; // Stores temperature value
  char humi[8]; // Stores humidity value

  /*
   * qos : 0 - at most one delivery (default)
   *       1 - Delivered at least once
   *       2 - Exactly one delivery
   */
  int qos = 0;

  /* 1 : Enable MQTT events */
  if (TYPE1SC.setMQTT_EV(1) == 0)
    DebugSerial.println("1.Enable MQTT events");

  /* 2 : Configure node parameters (ID, Address) */
  if (TYPE1SC.setMQTT_NODES(_NodeID, _IP) == 0)
    DebugSerial.println("2.Configure node parameter:ID, Address");

  /* 3 : Configure node parameters (Connection Timeout) */
  if (TYPE1SC.setMQTT_TIMEOUT(conn_timeout) == 0)
    DebugSerial.println("3.Configure node parameter:Timeout");

  /* 4 : Establish connection */
  if (TYPE1SC.MQTT_Connect() == 0)
    DebugSerial.println("4.Establish connection");

  /* 5 : Subscribe (register) to the topic on the endpoint */
  if (TYPE1SC.MQTT_SUBSCRIBE(qos, _Topic) == 0)
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

  memset(_message, 0x0, sizeof(_message));
  sprintf(_message, "Temperature/%s, Humidity/%s", temp, humi);
  /* 6 : Publish data to broker */
  if (TYPE1SC.MQTT_Publish(qos, _Topic, strlen(_message), _message) == 0)
    DebugSerial.println("6.Publish data to broker");

  /* 7 : UnSubscribe to the topic on the endpoint */
  if (TYPE1SC.MQTT_UnSUBSCRIBE(_Topic) == 0) {
    DebugSerial.println("7.UnSubscribe to the topic on the endpoint");
  } else {
    delay(1000);
    if (TYPE1SC.MQTT_UnSUBSCRIBE(_Topic) == 0) {
      DebugSerial.println("7.UnSubscribe to the topic on the endpoint");
    }
  }
  /* 8 : Disconnect MQTT Server */
  if (TYPE1SC.MQTT_DisConnect() == 0)
    DebugSerial.println("8.Disconnect MQTT Server");

  /* 0 : Disable MQTT events */
  if (TYPE1SC.setMQTT_EV(0) == 0)
    DebugSerial.println("9.Disable MQTT events");
}

void loop() { delay(1000); }
