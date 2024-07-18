#include "TYPE1SC.h"
#include <Arduino.h>
#include <U8x8lib.h>

#define DebugSerial Serial
#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19
#define EXT_ANT 4

#define DHTPIN 0
#include "DHT.h" /* https://github.com/markruys/arduino-DHT */
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);
HardwareSerial M1Serial(2); // use ESP32 UART2
TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);

#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
U8X8LOG u8x8log;

/* EXT_ANT_ON 0 : Use an internal antenna.
 * EXT_ANT_ON 1 : Use an external antenna.
 */
#define EXT_ANT_ON 0

void extAntenna() {
  if (EXT_ANT_ON) {
    pinMode(EXT_ANT, OUTPUT);
    digitalWrite(EXT_ANT, HIGH);
    delay(100);
  }
}

void setup() {
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8x8log.setRedrawMode(
      1); // 0: Update screen with newline, 1: Update screen for every char

  // put your setup code here, to run once:
  /* Serial2 Initialization */
  M1Serial.begin(115200, SERIAL_8N1, 16, 17); // RXD2 : 16, TXD2 : 17
  DebugSerial.begin(115200);

  DebugSerial.println("TYPE1SC Module Start!!!");
  u8x8log.print("TYPE1SC Module Start!!!\n");

  extAntenna();

  dht.begin();

  /* TYPE1SC Module Initialization */
  if (TYPE1SC.init()) {
    DebugSerial.println("TYPE1SC Module Error!!!");
    u8x8log.print("TYPE1SC Module Error!!!\n");
  }

  /* Network Registration Check */
  while (TYPE1SC.canConnect() != 0) {
    DebugSerial.println("Network not Ready!!!");
    u8x8log.print("Network not Ready!!!\n");
    delay(2000);
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");
  u8x8log.print("TYPE1SC Module Ready!!!\n");

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
  if (TYPE1SC.setMQTT_EV(1) == 0) {
    DebugSerial.println("1.Enable MQTT events");
    u8x8log.print("1.Enable MQTT events\n");
  }

  /* 2 : Configure node parameters (ID, Address) */
  if (TYPE1SC.setMQTT_NODES(_NodeID, _IP) == 0) {
    DebugSerial.println("2.Configure node parameter:ID, Address");
    u8x8log.print("2.Configure node parameter:ID, Address\n");
  }

  /* 3 : Configure node parameters (Connection Timeout) */
  if (TYPE1SC.setMQTT_TIMEOUT(conn_timeout) == 0) {
    DebugSerial.println("3.Configure node parameter:Timeout");
    u8x8log.print("3.Configure node parameter:Timeout\n");
  }

  /* 4 : Establish connection */
  if (TYPE1SC.MQTT_Connect() == 0) {
    DebugSerial.println("4.Establish connection");
    u8x8log.print("4.Establish connection\n");
  }

  /* 5 : Subscribe (register) to the topic on the endpoint */
  if (TYPE1SC.MQTT_SUBSCRIBE(qos, _Topic) == 0) {
    DebugSerial.println("5.Subscribe to the topic on the endpoint");
    u8x8log.print("5.Subscribe to the topic on the endpoint\n");
  }

  /*Get Temperature & Humidity */
  while (1) {
    /* Get DHT22 Sensor */
    t = dht.readTemperature();
    h = dht.readHumidity();
    if (String(t) != "nan" && String(h) != "nan")
      break;
    else {
      DebugSerial.println("case nan ...");
      u8x8log.print("case nan ...\n");
      delay(1000);
    }
  }

  dtostrf(t, 4, 1, temp);
  dtostrf(h, 4, 1, humi);

  memset(_message, 0x0, sizeof(_message));
  sprintf(_message, "Temperature/%s, Humidity/%s", temp, humi);
  /* 6 : Publish data to broker */
  if (TYPE1SC.MQTT_Publish(qos, _Topic, strlen(_message), _message) == 0) {
    DebugSerial.println("6.Publish data to broker");
    u8x8log.print("6.Publish data to broker\n");
  }

  /* 7 : UnSubscribe to the topic on the endpoint */
  if (TYPE1SC.MQTT_UnSUBSCRIBE(_Topic) == 0) {
    DebugSerial.println("7.UnSubscribe to the topic on the endpoint");
    u8x8log.print("7.UnSubscribe to the topic on the endpoint\n");
  } else {
    delay(1000);
    if (TYPE1SC.MQTT_UnSUBSCRIBE(_Topic) == 0) {
      DebugSerial.println("7.UnSubscribe to the topic on the endpoint");
      u8x8log.print("7.UnSubscribe to the topic on the endpoint\n");
    }
  }
  /* 8 : Disconnect MQTT Server */
  if (TYPE1SC.MQTT_DisConnect() == 0) {
    DebugSerial.println("8.Disconnect MQTT Server");
    u8x8log.print("8.Disconnect MQTT Server\n");
  }

  /* 9 : Disable MQTT events */
  if (TYPE1SC.setMQTT_EV(0) == 0) {
    DebugSerial.println("9.Disable MQTT events");
    u8x8log.print("9.Disable MQTT events\n");
  }

  /* 10 :Detach Network */
  if (TYPE1SC.setCFUN(0) == 0) {
    DebugSerial.println("10.detach Network!!!");
    u8x8log.print("10.detach Network!!!\n");
  }
  delay(10000); // Detach Setup Time : 10sec
}

void loop() { delay(1000); }
