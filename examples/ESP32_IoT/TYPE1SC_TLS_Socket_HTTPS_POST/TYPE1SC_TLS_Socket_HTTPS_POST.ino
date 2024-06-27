#include "TYPE1SC.h"
#include <Arduino.h>
#include <U8x8lib.h>

#define DebugSerial Serial
#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19
#define EXT_ANT 4

#define PROFILE_ID 2

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);
HardwareSerial M1Serial(2); // use ESP32 UART2
TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);

int recvCnt = 0;

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
	u8x8log.setRedrawMode(1); // 0: Update screen with newline, 1: Update screen for every char

	// put your setup code here, to run once:
	/* Serial2 Initialization */
	M1Serial.begin(115200, SERIAL_8N1, 16, 17); // RXD2 : 16, TXD2 : 17
	DebugSerial.begin(115200);
	DebugSerial.println("TYPE1SC Module Start!!!");
	u8x8log.print("TYPE1SC Module Start!!!\n");

	extAntenna();
	/* Board Reset */
	TYPE1SC.reset();
	delay(2000);

	/* TYPE1SC Module Initialization */
	if (TYPE1SC.init()) {
		DebugSerial.println("TYPE1SC Module Error!!!");
		u8x8log.print("TYPE1SC Module Error!!!\n");
	}
	/* Network Regsistraiton Check */
	while (TYPE1SC.canConnect() != 0) {
		DebugSerial.println("Network not Ready !!!");
		u8x8log.print("Network not Ready !!!\n");
		delay(2000);
	}
	DebugSerial.println("TYPE1SC Module Ready!!!");
	u8x8log.print("TYPE1SC Module Ready!!!\n");

	/* Enter a DNS address */
	char IPAddr[] = "httpbin.org";
	int _PORT = 443;	/* https default port */
	char sckInfo[128];
	char recvBuffer[1500];
	int recvSize = 0;

	/* 0 :Socket All Event Enable */
	if (TYPE1SC.setSocket_EV(1) == 0) {
		DebugSerial.println("Socket All Event Enable!!!");
		u8x8log.print("Socket All Event Enable!!!\n");
	}

	/* 1 :TCP Socket Create ( 0:UDP, 1:TCP ) */
	if (TYPE1SC.socketCreate(1, IPAddr, _PORT) == 0) {
		DebugSerial.println("TCP Socket Create!!!");
		u8x8log.print("TCP Socket Create!!!\n");
	}

	/* 2 :Socket SSL Enabel */
	int tlsProfileNumber = PROFILE_ID;
	if (TYPE1SC.socketSSL(tlsProfileNumber) == 0) {
		DebugSerial.println("Socket SSL Enable!!!");
		u8x8log.print("Socket SSL Enable!!!\n");
	}

INFO:
	/* 3 :TLS TCP-Socket Activation */
	if (TYPE1SC.socketActivate() == 0) {
		DebugSerial.println("TCP Socket Activation!!!");
		u8x8log.print("TCP Socket Activation!!!\n");
	}

	if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
		DebugSerial.print("Socket Info : ");
		u8x8log.print("Socket Info : ");
		DebugSerial.println(sckInfo);
		u8x8log.print(sckInfo);
		u8x8log.print("\n");
		if (strcmp(sckInfo, "ACTIVATED")) {
			TYPE1SC.socketDeActivate();
			delay(3000);
			goto INFO;
		}
	}

	/* 4 :TLS TCP-Socket Send Data */
	String wDB = "{\"email\": \"eve.holt@reqres.in\",\"password\": \"pistol\"}";

	String data = "POST /post HTTP/1.1\r\n";
	data += "Host: httpbin.org\r\n";
	data += "Accept:application/json\r\n";
	data += "Content-Type:application/json\r\n";
	data += "Content-Length:" + String(wDB.length()) + "\r\n";
	data += "\r\n" + wDB;

	if (TYPE1SC.socketSend(data.c_str()) == 0) {
		DebugSerial.println("[--- HTTPS Send ---]");
		u8x8log.print("[--- HTTPS Send ---]\n");
		DebugSerial.println(data);
		u8x8log.print(data);
		u8x8log.print("\n");
	} else{
		DebugSerial.println("Send Fail!!!");
		u8x8log.print("Send Fail!!!\n");
	}

	/* 5 :TLS TCP-Socket Receive Data */
	if (TYPE1SC.socketRecvHTTP(recvBuffer, sizeof(recvBuffer), &recvSize) == 0) {
		DebugSerial.println("[--- HTTPS Receive Start ---]");
		u8x8log.print("[--- HTTPS Receive Start ---]\n");
		//DebugSerial.print("[RecvSize] >> ");
		//DebugSerial.println(recvSize);
		//DebugSerial.print("[Recv");
		//DebugSerial.print(recvCnt++);
		//DebugSerial.print("] >> ");
		DebugSerial.println(recvBuffer);
		u8x8log.print(recvBuffer);
		u8x8log.print("\n");
		do{
			if (TYPE1SC.socketRecv(recvBuffer, sizeof(recvBuffer), &recvSize) == 0) {
				//DebugSerial.print("[RecvSize] >> ");
				//DebugSerial.println(recvSize);
				//DebugSerial.print("[Recv");
				//DebugSerial.print(recvCnt++);
				//DebugSerial.print("] >> ");
				DebugSerial.println(recvBuffer);
				u8x8log.print(recvBuffer);
				u8x8log.print("\n");
			}
		}while (recvSize > 0); 
	} else {
		DebugSerial.println("Recv Fail!!!");
		u8x8log.print("Recv Fail!!!\n");
	}
	DebugSerial.println("[--- HTTPS Receive End ---]");
	u8x8log.print("[--- HTTPS Receive End ---]\n");

	/* 6 :TLS TCP-Socket DeActivation */
	if (TYPE1SC.socketDeActivate() == 0){
		DebugSerial.println("TCP Socket DeActivation!!!");
		u8x8log.print("TCP Socket DeActivation!!!\n");
	}
	if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
		DebugSerial.print("Socket Info : ");
		u8x8log.print("Socket Info : ");
		DebugSerial.println(sckInfo);
		u8x8log.print(sckInfo);
		u8x8log.print("\n");
	}

	/* 7 :TLS TCP-Socket DeActivation */
	if (TYPE1SC.socketClose() == 0){
		DebugSerial.println("TCP Socket Close!!!");
		u8x8log.print("TCP Socket Close!!!\n");
	}

	/* 8 :Socket All Event Disable */
	if (TYPE1SC.setSocket_EV(0) == 0){
		DebugSerial.println("Socket All Event Disable!!!");
		u8x8log.print("Socket All Event Disable!!!\n");
	}

	delay(10000);
}
void loop() {
	delay(1000);
}
