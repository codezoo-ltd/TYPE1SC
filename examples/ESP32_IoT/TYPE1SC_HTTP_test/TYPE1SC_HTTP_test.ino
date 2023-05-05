#include "TYPE1SC.h"
#include <Arduino.h>
#include <U8x8lib.h>

#define DebugSerial Serial
#define M1Serial Serial2

#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19
#define EXT_ANT 4

#define DHTPIN 26
#include "DHT.h" /* https://github.com/markruys/arduino-DHT */
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);

#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
U8X8LOG u8x8log;

/* EXT_ANT_ON 0 : Use an internal antenna.
 * EXT_ANT_ON 1 : Use an external antenna.
 */
#define EXT_ANT_ON 0

void extAntenna(){
	if (EXT_ANT_ON)
	{
		pinMode(EXT_ANT, OUTPUT);
		digitalWrite(EXT_ANT, HIGH);
		delay(100);
	}
}

void setup() {
	u8x8.begin();
	u8x8.setFont(u8x8_font_chroma48medium8_r);

	u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
	u8x8log.setRedrawMode(1);    // 0: Update screen with newline, 1: Update screen for every char

	// put your setup code here, to run once:
	M1Serial.begin(115200);
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

	/* Network Regsistraiton Check */
	while (TYPE1SC.canConnect() != 0) {
		DebugSerial.println("Network not Ready!!!");
		u8x8log.print("Network not Ready!!!\n");
		delay(2000);
	}

	DebugSerial.println("TYPE1SC Module Ready!!!");
	u8x8log.print("TYPE1SC Module Ready!!!\n");

	/* Enter a DNS address to get an IP address */
	char IPAddr[32];

	while (1) {

		if (TYPE1SC.getIPAddr("api.thingspeak.com", IPAddr, sizeof(IPAddr)) == 0) {
			DebugSerial.print("IP Address : ");
			DebugSerial.println(IPAddr);
			u8x8log.print("IP Address : ");
			u8x8log.print(IPAddr);
			u8x8log.print("\n");			
			break;
		} else {
			DebugSerial.println("IP Address Error!!!");
			u8x8log.print("IP Address Error!!!\n");
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
			DebugSerial.println("case nan...");
			u8x8log.print("case nan...\n");
			delay(1000);
		}
	}

	/* 1 :TCP Socket Create ( 0:UDP, 1:TCP ) */
	if (TYPE1SC.socketCreate(1, IPAddr, _PORT) == 0){
		DebugSerial.println("TCP Socket Create!!!");
		u8x8log.print("TCP Socket Create!!!\n");
	}

INFO:

	/* 2 :TCP Socket Activation */
	if (TYPE1SC.socketActivate() == 0){
		DebugSerial.println("TCP Socket Activation!!!");
		u8x8log.print("TCP Socket Activation!!!\n");
	}

	if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
		DebugSerial.print("Socket Info : ");
		DebugSerial.println(sckInfo);
		u8x8log.print("Socket Info : ");
		u8x8log.print(sckInfo);
		u8x8log.print("\n");		

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
		DebugSerial.print("[HTTP Send] >> ");
		DebugSerial.println(data);
		u8x8log.print("[HTTP Send] >> ");
		u8x8log.print(data);
		u8x8log.print("\n");		
	} else {
		DebugSerial.println("Send Fail!!!");
		u8x8log.print("Send Fail!!!\n");
	}

	/* 4 :TCP Socket Recv Data */
	if (TYPE1SC.socketRecv(recvBuffer, sizeof(recvBuffer), &recvSize) == 0) {
		DebugSerial.print("[RecvSize] >> ");
		DebugSerial.println(recvSize);
		DebugSerial.print("[Recv] >> ");
		DebugSerial.println(recvBuffer);
		u8x8log.print("[RecvSize] >> ");
		u8x8log.print(recvSize);
		u8x8log.print("\n");
		u8x8log.print("[Recv] >> ");
		u8x8log.print(recvBuffer);
		u8x8log.print("\n");
	} else {
		DebugSerial.println("Recv Fail!!!");
		u8x8log.print("Recv Fail!!!\n");
	}

	/* 5 :TCP Socket DeActivation */
	if (TYPE1SC.socketDeActivate() == 0){
		DebugSerial.println("TCP Socket DeActivation!!!");
		u8x8log.print("TCP Socket DeActivation!!!\n");
	}

	if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
		DebugSerial.print("Socket Info : ");
		DebugSerial.println(sckInfo);
		u8x8log.print("Socket Info : ");
		u8x8log.print(sckInfo);
		u8x8log.print("\n");		
	}

	/* 6 :TCP Socket DeActivation */
	if (TYPE1SC.socketClose() == 0){
		DebugSerial.println("TCP Socket Close!!!");
		u8x8log.print("TCP Socket Close!!!\n");
	}

	/* 7 :Detach Network */
	if (TYPE1SC.setCFUN(0) == 0){
		DebugSerial.println("detach Network!!!");
		u8x8log.print("detach Network!!!\n");
	}
	delay(10000);   //Detach Setup Time : 10sec 	
}

void loop() { delay(1000); }
