/*
 * A library for controlling TYPE1SC LTE CAT.M1
 *
 * @Author Rooney.Jang  [CodeZoo]
 * @Date 05/06/2021
 *
 */

#include "TYPE1SC.h"
#include "Arduino.h"

extern "C" {
#include "stdlib.h"
#include "string.h"
#ifndef __AVR__
#include <cstdarg>
#endif
}

#define SWIR_TRACE(...) TYPE1SC_trace(__VA_ARGS__);
#define TRACE_BUFFER_SIZE 256
#define BG_LINE 30 // Limit 30 Line
//#define __TYPE_1SC_DEBUG		//Debug mode

TYPE1SC::TYPE1SC(Stream &serial, Stream &debug, uint8_t pwr_pin, uint8_t status_pin)
	: _serial(serial), _debug(debug), _pwr_pin(pwr_pin),
	_status_pin(status_pin) {
		_timeOut = 2000; /* default Timeout */
		_serial.setTimeout(
				(_timeOut + 500)); /* +500ms, Serial TX/RX Timeout default 2000 */
		pinMode(_pwr_pin, OUTPUT);
		pinMode(_status_pin, INPUT); /* PULL None */
	}

int TYPE1SC::pwrON() {
	int ret;

	delay(50); // Setup Time : Greater than or equal to 30ms
	digitalWrite(_pwr_pin, HIGH);
	delay(600); // Hold Time : Greater than or equal to 500ms
	digitalWrite(_pwr_pin, LOW);
	delay(5000); // Release Time : Greater than or equal to 4800ms

	ret = !(digitalRead(_status_pin));

	return ret;
}

int TYPE1SC::pwrOFF() {
	Countdown oCountDown(60000); /* Max Timeout 1 minute */
	int ret = 1;

	digitalWrite(_pwr_pin, HIGH);
	delay(800); // Hold Time : Greater than or equal to 650ms
	digitalWrite(_pwr_pin, LOW);
	delay(3000); // Release Time : Greater than or equal to 2000ms

	do {
		if (digitalRead(_status_pin)) {
			delay(1000);
		} else {
			ret = 0;
			break;
		}
	} while (!oCountDown.expired());

	return ret;
}

int TYPE1SC::isPwrON() {
	int ret;

	delay(50); // Setup Time : Greater than or equal to 30ms

	ret = digitalRead(_status_pin);

	return ret;
}

int TYPE1SC::init() {
	char szCmd[16];
	char resBuffer[16];
	int cnt = 0;
	int ret;

	TYPE1SC_serial_clearbuf();

	memset(resBuffer, 0, sizeof(resBuffer));
	strcpy(szCmd, "AT");

	do {

		ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK", 3000);

		if (ret == 0)
			break;

	} while (cnt < 10);

	strcpy(szCmd, "ATE0"); // Echo Off
	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK", 3000);

	strcpy(szCmd, "AT+CEREG=2");
	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK", 3000);

	return ret;
}

int TYPE1SC::getCCLK(char *szCCLK, int nBufferSize) {
	char szCmd[16];
	int ret;

	TYPE1SC_serial_clearbuf();

	memset(szCCLK, 0, nBufferSize);

	strcpy(szCmd, "AT+CCLK?");
	ret = sendATcmd(szCmd, szCCLK, nBufferSize, "+CCLK:");

	return ret;
}

int TYPE1SC::getCGMR(char *szCGMR, int nBufferSize) {
	char szCmd[16];
	char *aLine[BG_LINE]; // read up to 20 lines

	TYPE1SC_serial_clearbuf();

	strcpy(szCmd, "AT+CGMR");
	int nNbLine = sendATcmd(szCmd, aLine, BG_LINE);

	char *sLine;
	memset(szCGMR, 0, nBufferSize);

	for (int i = 0; i < nNbLine; i++) {
		sLine = aLine[i];

		SWIR_TRACE(F("getCGMR, line[%d]: %s\n"), i, sLine);

		char *pTemp = sLine;
		while (pTemp < (sLine + strlen(sLine))) // trim ending
		{
			if (*pTemp == '\r') // remove cariage return
			{
				*pTemp = 0; // zero terminate string
				break;
			}
			if (*pTemp == '\n') // remove cariage return
			{
				*pTemp = 0; // zero terminate string
				break;
			}
			pTemp++;
		}

		int nLen = strlen(sLine);
		if (nLen < 15) {
			free(aLine[i]);
			continue;
		}
		for (int k = 0; k < nLen; k++) {
			if (sLine[k] < '-' || sLine[k] > 'z') {
				continue;
			}
		}

		strcpy(szCGMR, sLine);
		free(aLine[i]);
	}

	return (strlen(szCGMR) > 0 ? 0 : 1);
}

int TYPE1SC::getCFUN(int *value) {
	char szCmd[16];
	char resBuffer[16];
	int ret, _value;

	TYPE1SC_serial_clearbuf();

	strcpy(szCmd, "AT+CFUN?");

	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "+CFUN:");

	_value = atoi(resBuffer);
	*value = _value;

	return ret;
}

int TYPE1SC::setCFUN(int value) {
	char szCmd[16];
	char resBuffer[16];
	int ret;

	TYPE1SC_serial_clearbuf();

	sprintf(szCmd, "AT+CFUN=%d", value);

	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK", 3000);

	return ret;
}

int TYPE1SC::getCIMI(char *szCIMI, int nBufferSize) {
	char szCmd[16];
	char *aLine[BG_LINE]; // read up to 20 lines

	TYPE1SC_serial_clearbuf();

	strcpy(szCmd, "AT+CIMI");
	int nNbLine = sendATcmd(szCmd, aLine, BG_LINE);

	char *sLine;
	memset(szCIMI, 0, nBufferSize);

	for (int i = 0; i < nNbLine; i++) {
		sLine = aLine[i];

		SWIR_TRACE(F("getCIMI, line[%d]: %s\n"), i, sLine);

		char *pTemp = sLine;
		while (pTemp < (sLine + strlen(sLine))) // trim ending
		{
			if (*pTemp == '\r') // remove cariage return
			{
				*pTemp = 0; // zero terminate string
				break;
			}
			if (*pTemp == '\n') // remove cariage return
			{
				*pTemp = 0; // zero terminate string
				break;
			}
			pTemp++;
		}

		int nLen = strlen(sLine);
		if (nLen != 15) {
			free(aLine[i]);
			continue;
		}
		for (int k = 0; k < nLen; k++) {
			if (sLine[k] < '0' || sLine[k] > '9') {
				continue;
			}
		}

		strcpy(szCIMI, sLine);
		free(aLine[i]);
	}

	return (strlen(szCIMI) > 0 ? 0 : 1);
}

int TYPE1SC::getIMEI(char *szIMEI, int nBufferSize) {
	char szCmd[16];
	char *aLine[BG_LINE]; // read up to 20 lines

	TYPE1SC_serial_clearbuf();

	strcpy(szCmd, "AT+CGSN");
	int nNbLine = sendATcmd(szCmd, aLine, BG_LINE);

	char *sLine;
	memset(szIMEI, 0, nBufferSize);

	for (int i = 0; i < nNbLine; i++) {
		sLine = aLine[i];

		SWIR_TRACE(F("getIMEI, line[%d]: %s\n"), i, sLine);

		char *pTemp = sLine;
		while (pTemp < (sLine + strlen(sLine))) // trim ending
		{
			if (*pTemp == '\r') // remove cariage return
			{
				*pTemp = 0; // zero terminate string
				break;
			}
			if (*pTemp == '\n') // remove cariage return
			{
				*pTemp = 0; // zero terminate string
				break;
			}
			pTemp++;
		}

		int nLen = strlen(sLine);
		if (nLen != 15) {
			free(aLine[i]);
			continue;
		}
		for (int k = 0; k < nLen; k++) {
			if (sLine[k] < '0' || sLine[k] > '9') {
				continue;
			}
		}

		strcpy(szIMEI, sLine);
		free(aLine[i]);
	}

	return (strlen(szIMEI) > 0 ? 0 : 1);
}

int TYPE1SC::isActPDP(char *szPDP, int nBufferSize) {
	char szCmd[16];
	int ret;

	TYPE1SC_serial_clearbuf();

	memset(szPDP, 0, nBufferSize);
	strcpy(szCmd, "AT+QIACT?");

	ret = sendATcmd(szCmd, szPDP, nBufferSize, "+QIACT:");

	return ret;
}

int TYPE1SC::actPDP() {
	char szCmd[16];
	char resBuffer[16];
	int ret;

	TYPE1SC_serial_clearbuf();

	strcpy(szCmd, "AT+QIACT=1,1");

	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK");

	delay(5000); // Maximum Response Time 150 seconds, determined by network

	return ret;
}

int TYPE1SC::deActPDP() {
	char szCmd[16];
	char resBuffer[16];
	int ret;

	TYPE1SC_serial_clearbuf();

	strcpy(szCmd, "AT+QIDEACT=1");

	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK");

	return ret;
}

int TYPE1SC::canConnect() {
	char szATcmd[16];
	char resBuffer[32];

	TYPE1SC_serial_clearbuf();

	sprintf(szATcmd, "AT+CEREG?");
	if (0 == sendATcmd(szATcmd, resBuffer, sizeof(resBuffer), "+CEREG")) {
		char *pszState = NULL;

		pszState = strstr(resBuffer, ",");
		if (pszState != NULL) {
			pszState++;
			if (*pszState == '1' || *pszState == '5') {
				SWIR_TRACE(F("ready to make data call..."));
				return 0; // ready to connect. CEREG equals to 1 (home) nor 5 (roaming)
			}
		}
	}

	SWIR_TRACE(F("Not ready to make data call..."));
	return 1; 
}

int TYPE1SC::getRSSI(int *rssi) {
	char szATcmd[16];
	char resBuffer[64];
	int  Rssi;

	TYPE1SC_serial_clearbuf();

	sprintf(szATcmd, "AT%%MEAS=\"8\"");
	if (0 == sendATcmd(szATcmd, resBuffer, sizeof(resBuffer),
				"RSSI = ", 4000)) {
		Rssi = atoi(resBuffer);

		SWIR_TRACE(F("RSSI %d \r\n"), Rssi);
		*rssi = Rssi;
		return 0;
	}

	SWIR_TRACE(F("get RSSI Fail..."));
	return 1;
}

int TYPE1SC::getTxPower(char *txPower, int txPowerSize) {
	char szCmd[16];
	int ret;

	TYPE1SC_serial_clearbuf();

	memset(txPower, 0, txPowerSize);

	strcpy(szCmd, "AT%MEAS=\"4\"");
	ret = sendATcmd(szCmd, txPower, txPowerSize, "TX power: ", 4000);

	return ret;
}

int TYPE1SC::getIPAddr(const char *srcAddr, char *dstAddr, int dstAddrSize) {
	char szCmd[128];
	char szRst[128];
	int ret;

	TYPE1SC_serial_clearbuf();

	memset(dstAddr, 0, dstAddrSize);

	sprintf(szCmd, "AT%%DNSRSLV=0,\"%s\"",srcAddr);
	ret = sendATcmd(szCmd, szRst, sizeof(szRst), "%DNSRSLV:0,\"", 10000);

	if(!ret)
	{
		char *pBuffer1, *pBuffer2;
		pBuffer1 = szRst;
		pBuffer2 = strstr(pBuffer1, "\"");

		if(pBuffer2 != NULL){
			memcpy(dstAddr, pBuffer1, pBuffer2-pBuffer1);
		}
	}

	return ret;
}

int TYPE1SC::getServingCell(int *servingCell) {
	char szATcmd[32];
	char resBuffer[128];
	int  ServingCell;

	TYPE1SC_serial_clearbuf();

	sprintf(szATcmd, "AT%%LBSCMD=\"MLIDS\"");
	if (0 == sendATcmd(szATcmd, resBuffer, sizeof(resBuffer), "%LBSCMD:")) {
		char *pszState = NULL;
		char *pszState2 = NULL;

		pszState2 = strstr(resBuffer, ",");
		if(pszState2 != NULL)
			pszState2++;
		else{
			SWIR_TRACE(F("get Serving Cell Fail..."));
			return 1;
		}

		for (int i = 0; i < 10; i++) {
			pszState = strstr(pszState2, ",");
			if (pszState != NULL) {
				pszState++;
				pszState2 = pszState;
			} else {
				SWIR_TRACE(F("get Serving Cell Fail..."));
				return 1;
			}
		}

		ServingCell = atoi(pszState2);

		SWIR_TRACE(F("Serving Cell Physical Cell ID %d \r\n"), ServingCell);
		*servingCell = ServingCell;
		return 0;
	}

	SWIR_TRACE(F("get Serving Cell Fail..."));
	return 1;
}

int TYPE1SC::getRSRQ(int *rsrq) {
	char szATcmd[16];
	char resBuffer[64];
	int  Rsrq;

	TYPE1SC_serial_clearbuf();

	sprintf(szATcmd, "AT%%MEAS=\"8\"");
	if (0 == sendATcmd(szATcmd, resBuffer, sizeof(resBuffer),
				"RSRQ = ", 4000)) {
		Rsrq = atoi(resBuffer);

		SWIR_TRACE(F("RSRQ %d \r\n"), Rsrq);
		*rsrq = Rsrq;
		return 0;
	}

	SWIR_TRACE(F("get RSRQ Fail..."));
	return 1;
}


int TYPE1SC::getRSRP(int *rsrp) {
	char szATcmd[16];
	char resBuffer[64];
	int  Rsrp;

	TYPE1SC_serial_clearbuf();

	sprintf(szATcmd, "AT%%MEAS=\"8\"");
	if (0 == sendATcmd(szATcmd, resBuffer, sizeof(resBuffer),
				"RSRP = ", 4000)) {
		Rsrp = atoi(resBuffer);

		SWIR_TRACE(F("RSRP %d \r\n"), Rsrp);
		*rsrp = Rsrp;
		return 0;
	}

	SWIR_TRACE(F("get RSRP Fail..."));
	return 1;
}

int TYPE1SC::getSINR(int *sinr) {
	char szATcmd[16];
	char resBuffer[64];
	int  Sinr;

	TYPE1SC_serial_clearbuf();

	sprintf(szATcmd, "AT%%MEAS=\"8\"");
	if (0 == sendATcmd(szATcmd, resBuffer, sizeof(resBuffer),
				"SINR = ", 4000)) {
		Sinr = atoi(resBuffer);

		SWIR_TRACE(F("SINR %d \r\n"), Sinr);
		*sinr = Sinr;
		return 0;
	}

	SWIR_TRACE(F("get SINR Fail..."));
	return 1;
}

int TYPE1SC::socketCreate(int service_type, char *remote_address,
		long remote_port) {
	char szCmd[128];
	char recvBuf[128];
	char *service[] = {"UDP", "TCP"};

	TYPE1SC_serial_clearbuf();

	sprintf(szCmd, "AT%%SOCKETCMD=\"ALLOCATE\",0,\"%s\",\"OPEN\",\"%s\",%d,1500",
			service[service_type], remote_address, remote_port);

	if (0 == sendATcmd(szCmd, recvBuf, sizeof(recvBuf), "SOCKETCMD:", 20000))
	{
		_nSocket = atoi(recvBuf);
		_nSocket = 1;	//Hard Cording (need Test by Murata or Altair)
		if (_nSocket > 4)
			_nSocket = 1;

		SWIR_TRACE(F("Socket : %d \r\n"), _nSocket);

		return 0;
	}

	SWIR_TRACE(F("socket Create Fail..."));
	return 1;
}

int TYPE1SC::socketActivate(){
	char szCmd[32];
	char resBuffer[32];
	int ret;

	TYPE1SC_serial_clearbuf();

	memset(resBuffer, 0, sizeof(resBuffer));

	sprintf(szCmd, "AT%%SOCKETCMD=\"ACTIVATE\",%d", _nSocket);
	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK", 5000);

	return ret;
}

int TYPE1SC::socketInfo(char *info, int infoSize){
	char szCmd[128];
	char resBuffer[128];
	int ret;

	TYPE1SC_serial_clearbuf();

	memset(info, 0, infoSize);

	sprintf(szCmd, "AT%%SOCKETCMD=\"INFO\",%d", _nSocket);

	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "SOCKETCMD:\"", 5000);
	if(!ret){
		char *pBuffer1, *pBuffer2;

		pBuffer1 = resBuffer;
		pBuffer2 = strstr(pBuffer1, "\"");

		if(pBuffer2 != NULL){
			memcpy(info, pBuffer1, pBuffer2-pBuffer1);
		}
	}

	return ret;
}

int TYPE1SC::socketDeActivate(){
	char szCmd[32];
	char resBuffer[32];
	int ret;

	TYPE1SC_serial_clearbuf();

	memset(resBuffer, 0, sizeof(resBuffer));

	sprintf(szCmd, "AT%%SOCKETCMD=\"DEACTIVATE\",%d", _nSocket);
	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK", 20000);

	return ret;
}

int TYPE1SC::socketClose() {
	char szCmd[32];
	char resBuffer[32];
	int ret;

	TYPE1SC_serial_clearbuf();

	memset(resBuffer, 0, sizeof(resBuffer));

	sprintf(szCmd, "AT%%SOCKETCMD=\"DELETE\",%d", _nSocket);
	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK", 20000);

	return ret;
}

#define NIBBLE_TO_HEX_CHAR(i) ((i <= 9) ? ('0' + i) : ('A' - 10 + i))
#define HIGH_NIBBLE(i) ((i >> 4) & 0x0F)
#define LOW_NIBBLE(i) (i & 0x0F)
int TYPE1SC::socketSend(char *buffer, int size) {
	char    resBuffer[32];
	char*   sendBuffer;
	char*   pSendBuffer;
	int     ret, pSeek, bufferSize, SendSize;

	_serial.setTimeout(20000);
	TYPE1SC_serial_clearbuf();

	if( size < 1 )
		return 1;

	bufferSize = (size * 2) + 64;

	sendBuffer = (char *)malloc(bufferSize);

	memset(sendBuffer, 0, bufferSize);

	sprintf(sendBuffer, "AT%%SOCKETDATA=\"SEND\",%d,%d,\"", _nSocket, size);

	pSeek = strlen(sendBuffer);
	SWIR_TRACE(F("sendBuffer (%s)..."), sendBuffer);
	SWIR_TRACE(F("pSeek [%d]\n"), pSeek);
	pSendBuffer = sendBuffer;
	pSendBuffer += pSeek;

	for (size_t i = 0; i < size; ++i) {
		*pSendBuffer = (static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(buffer[i]))));
		*pSendBuffer++;
		*pSendBuffer = (static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(buffer[i]))));
		*pSendBuffer++;
	}

	*pSendBuffer = '\"';

	_serial.println(sendBuffer);
//	_debug.println(sendBuffer);	/* Debug Data */

	free(sendBuffer);

	ret = readATresponseLine(resBuffer, sizeof(resBuffer), "OK", 20000);

	return ret;			
}

int TYPE1SC::socketSend(const char *str) {
	return socketSend((char *)str, strlen(str));
}

#define HEX_CHAR_TO_NIBBLE(c) ((c >= 'A') ? (c - 'A' + 0x0A) : (c - '0'))
#define HEX_PAIR_TO_BYTE(h, l) ((HEX_CHAR_TO_NIBBLE(h) << 4) + HEX_CHAR_TO_NIBBLE(l))
int TYPE1SC::socketRecv(char *buffer, int bufferSize, int *recvSize) {
	char    szCmd[32];
	char    resBuffer[1048];	/* Max 512 byte Receive */
	int		RecvSize,ret;
	char	*pszState = NULL;
	char	*pszState2 = NULL;

	_serial.setTimeout(20000);
	delay(3000);
	TYPE1SC_serial_clearbuf();

	memset(resBuffer, 0, sizeof(resBuffer));

	sprintf(szCmd, "AT%%SOCKETDATA=\"RECEIVE\",%d,1000", _nSocket);
	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "SOCKETDATA:", 20000);

	if(!ret){
		SWIR_TRACE(F("Recv %s\n"),resBuffer);

		pszState = strstr(resBuffer, ",");
		if(pszState!=NULL)
			pszState++;

		RecvSize = atoi(pszState);
		SWIR_TRACE(F("RecvSize [%d]\n"), RecvSize);

		*recvSize = RecvSize;

		pszState2 = pszState;

		for(int i=0; i<2; i++){
			pszState = strstr(pszState2, ",");
			if(pszState!=NULL){
				pszState++;
				pszState2 = pszState;
			}
		}

		pszState++; 
		pszState2 = pszState;
		pszState = strstr(pszState2, "\"");

		if(pszState != NULL){

			memset(buffer, 0, bufferSize);

			for (size_t i = 0; i < RecvSize * 2; i += 2) {
				buffer[i / 2] = HEX_PAIR_TO_BYTE(pszState2[i], pszState2[i + 1]);
			}
		}

	}

	return ret;
}

int TYPE1SC::sendATcmd(char *szCmd, char *szResponse, int nResponseBufSize,
		const char *szResponseFilter, unsigned long ulWaitDelay) {
	int nRet = 0;

	memset(szResponse, 0, nResponseBufSize);

	SWIR_TRACE(F("sendATcmd (%s) - %d..."), szCmd, ulWaitDelay);

	_serial.setTimeout(ulWaitDelay + 500);
	_serial.println(szCmd);

	nRet = readATresponseLine(szResponse, nResponseBufSize, szResponseFilter,
			ulWaitDelay);

	if (nRet == 0) {
		SWIR_TRACE(F("...sendATcmd OK"));
	} else {
		SWIR_TRACE(F("...sendATcmd Fails"));
	}

	return nRet;
}

int TYPE1SC::sendATcmd(char *szCmd, char *aLine[], int nMaxLine,
		unsigned long ulWaitDelay) {
	int nRet = 0;

	SWIR_TRACE(F("sendATcmd2 (%s) - %d..."), szCmd, ulWaitDelay);

	_serial.setTimeout(ulWaitDelay + 500);
	_serial.println(szCmd);

	nRet = readATresponseLine(aLine, nMaxLine, ulWaitDelay);

	if (nRet > 0) {
		SWIR_TRACE(F("...sendATcmd OK"));
	} else {
		SWIR_TRACE(F("...sendATcmd Fails"));
	}

	return nRet;
}

int TYPE1SC::readATresponseLine(char *szLine, int nLineBufSize,
		const char *szResponseFilter,
		unsigned long ulDelay) {
	int nbLine = 0;
	char *aLine[BG_LINE];
	Countdown oCountdown(ulDelay);
	char *pszSubstr = NULL;

	memset(szLine, 0, nLineBufSize);

	do {
		if (_serial.available()) {
			String sStr;
			sStr = _serial.readStringUntil('\n');
			int nLen = sStr.length();

			if (nLen > 1) {
				aLine[nbLine] = (char *)malloc(nLen + 1);
				sStr.toCharArray(aLine[nbLine], nLen);
				aLine[nbLine][nLen] = 0;

				pszSubstr = strstr(aLine[nbLine], "OK");
				if (pszSubstr != NULL) {
					nbLine++;
					SWIR_TRACE(F("Found OK"));
					break;
				}

				nbLine++;
			}
		}
		if (nbLine >= BG_LINE) {
			break;
		}
	} while (!oCountdown.expired());

	SWIR_TRACE(F("readATresponseLine: %d line(s)\n"), nbLine);

	int i;

	int nRet = 3;

	for (i = 0; i < nbLine; i++) {
		SWIR_TRACE(F("line[%d]: %s\n"), i, aLine[i]);

		if (szResponseFilter == NULL) {
			// Not filtering response
			strcpy(szLine, aLine[i]);
			nRet = 0;
		} else if (strlen(szResponseFilter) > 0) {
			// Filtering Response
			char *pszSubstr = NULL;

			pszSubstr = strstr(aLine[i], szResponseFilter);
			if (pszSubstr != NULL) {
				pszSubstr += strlen(szResponseFilter);
				while (isSpace(*pszSubstr)) // trim heading
				{
					pszSubstr++;
				}
				char *pTemp = pszSubstr;
				while (pTemp < (aLine[i] + strlen(aLine[i]))) // trim ending
				{
					if (*pTemp == '\n') // remove cariage return
					{
						*pTemp = 0; // zero terminate string
						break;
					}
					pTemp++;
				}

				SWIR_TRACE(F("Filtered response: %s\n"), pszSubstr);
				strcpy(szLine, pszSubstr);
				nRet = 0;
			}
		} else {
			// Not filtering response
			strcpy(szLine, aLine[i]);
			nRet = 0;
		}

		free(aLine[i]);
	}

	return nRet;
}

int TYPE1SC::readATresponseLine(char *aLine[], int nMaxLine,
		unsigned long ulDelay) {
	int nbLine = 0;
	Countdown oCountDown(ulDelay);
	char *pszSubstr = NULL;
	int i;

	do {
		if (_serial.available()) {
			String sStr;
			sStr = _serial.readStringUntil('\n');

			int nLen = sStr.length();
			if (nLen > 1) {
				aLine[nbLine] = (char *)malloc(nLen + 1);
				sStr.toCharArray(aLine[nbLine], nLen);

				aLine[nbLine][nLen] = 0;

				pszSubstr = strstr(aLine[nbLine], "OK");
				if (pszSubstr != NULL) {
					nbLine++;
					SWIR_TRACE(F("Found OK"));
					break;
				}

				nbLine++;
			}
		}
		if (nbLine >= nMaxLine) {
			break;
		}
	} while (!oCountDown.expired());

	SWIR_TRACE(F("readATresponseLine2: %d line(s)\n"), nbLine);

	return nbLine;
}

void TYPE1SC::TYPE1SC_serial_clearbuf() {

	while (_serial.available()) {
		_serial.read();
	}
}

void TYPE1SC::TYPE1SC_trace(const __FlashStringHelper *szTrace, ...) {
#ifdef __TYPE_1SC_DEBUG
	char szBuf[TRACE_BUFFER_SIZE];
	va_list args;

	va_start(args, szTrace);
#ifdef __AVR__
	vsnprintf_P(szBuf, sizeof(szBuf), (const char *)szTrace,
			args); // program for AVR
#else
	vsnprintf(szBuf, sizeof(szBuf), (const char *)szTrace,
			args); // for the rest of the world
#endif
	va_end(args);

	_debug.println(szBuf);
#endif
}

int TYPE1SC::disablePSM() {
	char szCmd[16];
	char resBuffer[16];
	int ret;

	int value = 0;

	TYPE1SC_serial_clearbuf();

	sprintf(szCmd, "AT+CPSMS=%d", value);

	ret = sendATcmd(szCmd, resBuffer, sizeof(resBuffer), "OK", 3000);

	return ret;
}
