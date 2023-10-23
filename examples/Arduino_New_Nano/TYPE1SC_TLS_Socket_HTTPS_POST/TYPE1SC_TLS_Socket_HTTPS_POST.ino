#include "TYPE1SC.h"

#define DebugSerial Serial
#define M1Serial Serial1

#define PROFILE_ID 2

TYPE1SC TYPE1SC(M1Serial, DebugSerial);

int recvCnt = 0;

void setup() {
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

	/* Enter a DNS address */
	char IPAddr[] = "httpbin.org";
	int _PORT = 443;	/* https default port */
	char sckInfo[128];
	char recvBuffer[1500];
	int recvSize = 0;

	/* 0 :Socket All Event Enable */
	if (TYPE1SC.setSocket_EV(1) == 0)
		DebugSerial.println("Socket All Event Enable!!!");

	/* 1 :TCP-Socket Create ( 0:UDP, 1:TCP ) */
	if (TYPE1SC.socketCreate(1, IPAddr, _PORT) == 0)
		DebugSerial.println("TCP Socket Create!!!");

	/* 2 :Socket SSL Enabel */
	int tlsProfileNumber = PROFILE_ID;
	if (TYPE1SC.socketSSL(tlsProfileNumber) == 0) {
		DebugSerial.println("Socket SSL Enable!!!");
	}

INFO:

	/* 3 :TLS TCP-Socket Activation */
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
		DebugSerial.println(data);
	} else
		DebugSerial.println("Send Fail!!!");

	/* 5 :TLS TCP-Socket Recv Data */
	if (TYPE1SC.socketRecvHTTP(recvBuffer, sizeof(recvBuffer), &recvSize) == 0) {
		DebugSerial.println("[--- HTTPS Receive Start ---]");
		//DebugSerial.print("[RecvSize] >> ");
		//DebugSerial.println(recvSize);
		//DebugSerial.print("[Recv");
		//DebugSerial.print(recvCnt++);
		//DebugSerial.print("] >> ");
		DebugSerial.println(recvBuffer);
		do{
			if (TYPE1SC.socketRecv(recvBuffer, sizeof(recvBuffer), &recvSize) == 0) {
				//DebugSerial.print("[RecvSize] >> ");
				//DebugSerial.println(recvSize);
				//DebugSerial.print("[Recv");
				//DebugSerial.print(recvCnt++);
				//DebugSerial.print("] >> ");
				DebugSerial.println(recvBuffer);
			}
		}while (recvSize > 0); 
	} else {
		DebugSerial.println("Recv Fail!!!");
	}
	DebugSerial.println("[--- HTTPS Receive End ---]");

	/* 6 :TLS TCP-Socket DeActivation */
	if (TYPE1SC.socketDeActivate() == 0)
		DebugSerial.println("TCP Socket DeActivation!!!");

	if (TYPE1SC.socketInfo(sckInfo, sizeof(sckInfo)) == 0) {
		DebugSerial.print("Socket Info : ");
		DebugSerial.println(sckInfo);
	}

	/* 7 :TLS TCP-Socket DeActivation */
	if (TYPE1SC.socketClose() == 0)
		DebugSerial.println("TCP Socket Close!!!");

	/* 8 :Socket All Event Disable */
	if (TYPE1SC.setSocket_EV(0) == 0)
		DebugSerial.println("Socket All Event Disable!!!");

	delay(10000);

}

void loop() {
	delay(1000);
}
