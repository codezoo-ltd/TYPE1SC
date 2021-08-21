#include "BG96.h"

#define DebugSerial Serial
#define M1Serial Serial1
#define PWR_PIN 2
#define STAT_PIN 3

BG96 BG96(M1Serial, DebugSerial, PWR_PIN, STAT_PIN);
int value = 0;

void setup() {
  // put your setup code here, to run once:
  M1Serial.begin(115200);
  DebugSerial.begin(115200);

  /* Power On Sequence */
  if (BG96.isPwrON()) {
    DebugSerial.println("BG96 Power ON Status");
    if (BG96.pwrOFF()) {
      DebugSerial.println("BG96 Power Off Error");
    } else {
      DebugSerial.println("BG96 Power Off Success");
      DebugSerial.println("Module Power ON Sequence Start");
      if (BG96.pwrON()) {
        DebugSerial.println("BG96 Power ON Error");
      } else
        DebugSerial.println("BG96 Power ON Success");
    }
  } else {
    DebugSerial.println("BG96 Power OFF Status");
    if (BG96.pwrON()) {
      DebugSerial.println("BG96 Power ON Error");
    } else
      DebugSerial.println("BG96 Power ON Success");
  }

  /* BG96 Module Initialization */
  if (BG96.init()) {
    DebugSerial.println("BG96 Module Error!!!");
  }

  /* Network Regsistraiton Check */
  while (BG96.canConnect() != 0) {
    DebugSerial.println("Network not Ready !!!");
    delay(2000);
  }

  if (BG96.actPDP()) {
    DebugSerial.println("BG96 PDP Activation Error");
  }

  /* Network QoS Report to LGU+ (No-charge packet) */
  //	if (BG96.reportDevice() == 0)
  //	{
  //		DebugSerial.println("BG96 Device Report Success!!!");
  //	}

  delay(2000);

  DebugSerial.println("BG96 Module Ready!!!");

#if 0
	/* Get Phone Number */
	char szCIMI[16];
	if (BG96.getCIMI(szCIMI, sizeof(szCIMI)) == 0)
	{
		DebugSerial.print("CIMI : ");
		DebugSerial.println(szCIMI);
	}

	/* Get IMEI */
	char szIMEI[16];
	if (BG96.getIMEI(szIMEI, sizeof(szIMEI)) == 0)
	{
		DebugSerial.print("IMEI : ");
		DebugSerial.println(szIMEI);
	}

	/* Get Fimrware version */
	char szCGMR[20];
	if (BG96.getCGMR(szCGMR, sizeof(szCGMR)) == 0)
	{
		DebugSerial.print("CGMR : ");
		DebugSerial.println(szCGMR);
	}

	/* Get ServingCell */
	char servingCell[10];
	if (BG96.getServingCell(servingCell) == 0)
	{
		DebugSerial.print("ServingCell : ");
		DebugSerial.println(servingCell);
	}

	/* Get RSRP */
	int rsrp;

	if (BG96.getRSRP(&rsrp) == 0)
	{
		DebugSerial.print("RSRP : ");
		DebugSerial.println(rsrp);
	}

	/* Get SINR */
	int sinr;

	if (BG96.getSINR(&sinr) == 0)
	{
		DebugSerial.print("SINR : ");
		DebugSerial.println(sinr);
	}

	/* Get SINR */
	int txPower;

	if (BG96.getTxPower(&txPower) == 0)
	{
		DebugSerial.print("TX Power : ");
		DebugSerial.println(txPower);
	}

	/* Get RSSI */
	int rssi;

	if (BG96.getRSSI(&rssi) == 0)
	{
		DebugSerial.print("RSSI : ");
		DebugSerial.println(rssi);
	}

	/* Get Time (GMT, (+36/4) ==> Korea +9hour) */
	char szTime[32];
	if (BG96.getCCLK(szTime, sizeof(szTime)) == 0)
	{
		DebugSerial.print("Time : ");
		DebugSerial.println(szTime);
	}

	/* Activate/Deactivate PDP Context */
	char szPDP[32];
	if ( BG96.isActPDP(szPDP, sizeof(szPDP)) )
	{
		if (BG96.actPDP() == 0)
		{
			if (BG96.isActPDP(szPDP, sizeof(szPDP)) == 0)
			{
				DebugSerial.print("Activation PDP info : ");
				/* 1(ContextID), 1(Activated), 1(IPV4), IP Addr(xxx.xxx.xxx.xxx) */
				DebugSerial.println(szPDP);
			}
		}
	}

	if (BG96.deActPDP() == 0)
	{
		DebugSerial.println("Deactivation PDP Context");
	}

#endif

#if 0

	/* 3. CFUN setting 0: disable, 1: Full Function */
	if (value == 0)
		value = 1;
	else
		value = 0;

	if (BG96.setCFUN(value) == 0)
		DebugSerial.println("CFUN set OK");

	delay(2000);

	/* Network Check */
	if (BG96.getCFUN(&value) == 0)
	{
		DebugSerial.print("CFUN get : ");
		DebugSerial.println(value);
	}
#endif
  char _IP[] = "echo.mbedcloudtesting.com";
  int _PORT = 7;
  char sendBuffer[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  char sendBuffer2[] = "Hello CodeZoo!!!";
  char recvBuffer[32];
  int recvSize;

  /* 0: UDP, 1: TCP */
  if (BG96.socketCreate(0, _IP, _PORT) == 0)
    DebugSerial.println("Socket Create!!!");

  if (BG96.socketSend(sendBuffer, sizeof(sendBuffer)) == 0)
    DebugSerial.println("Socket Send!!!");

  if (BG96.socketRecv(recvBuffer, sizeof(recvBuffer), &recvSize, 5000) == 0) {
    for (int i = 0; i < recvSize; i++)
      DebugSerial.print(recvBuffer[i], DEC);
    DebugSerial.println(" << Read Data");
    DebugSerial.println("Socket Read!!!");
  }

  if (BG96.socketSend(sendBuffer2) == 0)
    DebugSerial.println("Socket Send!!!");

  if (BG96.socketRecv(recvBuffer, sizeof(recvBuffer), &recvSize, 5000) == 0) {
    DebugSerial.print(recvBuffer);
    DebugSerial.println(" << Read Data");
    DebugSerial.println("Socket Read!!!");
  }

  if (BG96.socketClose() == 0) {
    DebugSerial.println("Socket Close!!!");
  }

  if (BG96.setCFUN(0) == 0) {
    DebugSerial.println("Network Detach");
  }
}

void loop() { delay(120000); /* 2min */ }
