#include "TYPE1SC.h"

#define DebugSerial Serial
#define M1Serial Serial1  //Arduino 

#define PWR_PIN 2
#define STAT_PIN 3

TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, STAT_PIN);

void setup() {
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

  /* Network Regsistraiton Check */
  while (TYPE1SC.canConnect() != 0) {
    DebugSerial.println("Network not Ready !!!");
    delay(2000);
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");

}

void loop() {

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
#if 0  
  /* Enter a DNS address to get an IP address */
  char IPAddr[32];
  if (TYPE1SC.getIPAddr("echo.mbedcloudtesting.com", IPAddr, sizeof(IPAddr)) == 0) {
    DebugSerial.print("IP Address : ");
    DebugSerial.println(IPAddr);
  }
  delay(1000);
#endif  

  char _IP[] = "broker.hivemq.com";
  char _NodeID[] = "Cellular_node";
  char _Topic[] = "type1sc/0/test";
  char _message[64];
  uint32_t conn_timeout = 1200;
  /*  
   * qos : 0 - at most one delivery (default)
   *       1 - Delivered at least once
   *       2 - Exactly one delivery  
  */
  int qos = 0; 

  memset(_message, 0x0, sizeof(_message));

  /* Get Current Time */
  TYPE1SC.getCCLK(_message, sizeof(_message));

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

  /* 6 : Publish data to broker */
  if (TYPE1SC.MQTT_Publish(qos, _Topic, strlen(_message), _message) == 0) 
    DebugSerial.println("6.Publish data to broker");

  /* 7 : UnSubscribe to the topic on the endpoint */
  if (TYPE1SC.MQTT_UnSUBSCRIBE(_Topic) == 0){ 
    DebugSerial.println("7.UnSubscribe to the topic on the endpoint");
  }else{
    delay(1000);
    if (TYPE1SC.MQTT_UnSUBSCRIBE(_Topic) == 0){
      DebugSerial.println("7.UnSubscribe to the topic on the endpoint");
    }    
  }        
  /* 8 : Disconnect MQTT Server */
  if (TYPE1SC.MQTT_DisConnect() == 0) 
    DebugSerial.println("8.Disconnect MQTT Server");  

  /* 0 : Disable MQTT events */
  if (TYPE1SC.setMQTT_EV(0) == 0)
    DebugSerial.println("9.Disable MQTT events");      

  DebugSerial.println("/=========================================/");
  DebugSerial.println("");
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(20000);
}
