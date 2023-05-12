/*
 * A library for controlling TYPE1SC LTE CAT.M1
 *
 * @Author Rooney.Jang  [CodeZoo]
 * @Date 05/06/2021
 *
 */

#ifndef TYPE1SC_h
#define TYPE1SC_h

#include "Countdown.h"

class TYPE1SC {
	public:
		/*
		 * A simplified constructor taking only a Stream ({Software/Hardware}Serial)
		 * object. The serial port should already be initialised when initialising
		 * this library.
		 */
		TYPE1SC(Stream &serial, Stream &debug);
		TYPE1SC(Stream &serial, Stream &debug, uint8_t pwr_pin, uint8_t reset_pin,
				uint8_t wakeup_pin);

		/*
		 * Power on the module.
		 */
		void pwrON(void);

		/*
		 * Power off the module.
		 */
		void pwrOFF(void);

		/*
		 * ext Antenna Enable.
		 */
		void extAntON(uint8_t extAnt_pin);

		/*
		 * Initialization the module.
		 */
		int init(void);

		/*
		 * Return current date & time.
		 */
		int getCCLK(char *szCCLK, int nBufferSize);

		/*
		 * Request Manufacturer Revision.
		 */
		int getCGMR(char *szCGMR, int nBufferSize);

		/*
		 * Request Product Serial Number (returns the IMEI).
		 */
		int getIMEI(char *szIMEI, int nBufferSize);

		/*
		 * Request ICCID from the SIM (returns the ICCID).
		 */
		int getICCID(char *szICCID, int nBufferSize);

		/*
		 * Request Change device functionality status. (0-1)
		 */
		int getCFUN(int *value);

		/*
		 * Set Change device functionality. (0-1)
		 */
		int setCFUN(int value);

		/*
		 * Set Change HTTP Event functionality. (0-1)
		 */
		int setHTTP_EV(int value);

		/*
		 * Set HTTP Profile ID & Connection Address.
		 */
		int setHTTP_NODES(int profile_id, char *http_addr);

		/*
		 * Set HTTP TLS.
		 */
		int setHTTP_TLS(int profile_id, int nProfile);

		/*
		 * HTTP Get.
		 */
		int HTTP_GET(int profile_id, char *http_get_addr, int *readSize);

		/*
		 * HTTP Get.
		 */
		int HTTP_GET(int profile_id, char *http_get_addr, int *readSize, bool secure);

		/*
		 * HTTP Data Read.
		 */
		int HTTP_READ(int profile_id, int readSize, char* httpData, int nBufferSize);

		/*
		 * Set Change MQTT Event functionality. (0-1)
		 */
		int setMQTT_EV(int value);

		/*
		 * Set MQTT Client ID & Connection Address.
		 */
		int setMQTT_NODES(char *client_id, char *mqtt_addr);

		/*  
		 * Set MQTT Client ID & Connection Address & User ID & Password
		 */
		int setMQTT_NODES(char *client_id, char *mqtt_addr, char *mqtt_user, char *mqtt_pass);

		/*  
		 * Set MQTT Client ID & Connection Address & User ID & Password
		 */
		int setMQTT_NODES(char *client_id, char *mqtt_addr, char *mqtt_user);

		/*
		 * Set MQTT Session Timeout. (1 - 65535, Default 600 sec)
		 */
		int setMQTT_TIMEOUT(uint32_t value);

		/*
		 * MQTT Server Connect
		 */
		int MQTT_Connect(void);

		/*
		 * MQTT Server DisConnect
		 */
		int MQTT_DisConnect(void);

		/*
		 * MQTT Subscribe Topic
		 * qos : 0 - at most one delivery (default)
		 *       1 - Delivered at least once
		 *       2 - Exactly one delivery
		 */
		int MQTT_SUBSCRIBE(int qos, char *topic);

		/*
		 * MQTT UnSubscribe Topic
		 */
		int MQTT_UnSUBSCRIBE(char *topic);

		/*
		 * MQTT Publish Topic
		 * qos : 0 - at most one delivery (default)
		 *       1 - Delivered at least once
		 *       2 - Exactly one delivery
		 */
		int MQTT_Publish(int qos, char *topic, int szData, char *Data);

		/*
		 * Set Change AWS_IOT Event functionality. (0-1)
		 */
		int setAWSIOT_EV(int value);

		/*
		 * Set AWS_IOT Connection Property
		 */
		int setAWSIOT_CONN(char *client_id, char *aws_addr, int tlsProfileNo);

		/*
		 * Set AWS_IOT Timeout. (1 - 1200, Defalut 1200 sec)
		 */
		int setAWSIOT_TIMEOUT(int value);

		/*
		 * AWS_IOT Connect
		 */
		int AWSIOT_Connect(void);

		/*
		 * AWS_IOT DisConnect
		 */
		int AWSIOT_DisConnect(void);

		/*
		 * AWS_IOT Subscribe Topic
		 */
		int AWSIOT_SUBSCRIBE(char *topic);

		/*
		 * AWS_IOT UnSubscribe Topic
		 */
		int AWSIOT_UnSUBSCRIBE(char *topic);

		/*
		 * AWS_IOT Publish Topic
		 */
		int AWSIOT_Publish(char *topic, char *Data);

		/*
		 * Set Change device APN.
		 */
		int setAPN(char *apn);

		/*
		 * Get device APN.
		 */
		int getAPN(char *apn, int bufferSize);

		/*
		 * Set PPP mode.
		 */
		int setPPP(void);

		/*
		 * Set AT mode.
		 */
		int setAT(void);

		/*
		 * Write Secure KEY.
		 */
		int writeKEY(const char *fileName, int isKEY, const char *key);

		/*
		 * ADD Certification Profile. range (1-255)
		 */
		int addCert(int nProfile);

		/*
		 * ADD HTTP Certification Profile. range (1-255)
		 */
		int addHTTPCert(int nProfile);

		/*
		 * DELETE Certification Profile. range (1-255)
		 */
		int delCert(int nProfile);

		/*
		 * Request international mobile subscriber identity.
		 */
		int getCIMI(char *szCIMI, int nBufferSize);

		/*
		 * EPS network registration status.
		 */
		int canConnect(void);

		/*
		 * Get REJECT Cause Number.
		 */
		int getRejectCause(int *rejectNum);

		/*
		 * Get Serving Cell Number.
		 */
		int getServingCell(int *servingCell);

		/*
		 * Get RSRP(Reference Signal Received Power) information.
		 */
		int getRSRP(int *rsrp);

		/*
		 * Get RSRQ(Reference Signal Received Quality) information.
		 */
		int getRSRQ(int *rsrq);

		/*
		 * Get SINR(Signal Interference plus Noise Ratio) information.
		 */
		int getSINR(int *sinr);

		/*
		 * Get RSSI(Received Signal Strength Indicator) information.
		 */
		int getRSSI(int *rssi);

		/*
		 * Get TX power.
		 */
		int getTxPower(char *txPower, int txPowerSize);

		/*
		 * Enter a DNS address to get an IP address.
		 */
		int getIPAddr(const char *srcAddr, char *dstAddr, int dstAddrSize);

		/*
		 * Create UDP/TCP Socket.
		 * service_type : 0(UDP) or 1(TCP)
		 * remote_address : IP_address or domain_name
		 * remote_port : (0-65535)
		 */
		int socketCreate(int service_type, char *remote_address, long remote_port);

		/* Socket Activation */
		int socketActivate(void);

		/* Socket DeActivation */
		int socketDeActivate(void);

		/* Socket Information */
		int socketInfo(char *info, int infoSize);

		/*
		 * Close UDP/TCP Socket.
		 */
		int socketClose(void);

		/*
		 * Send UDP/TCP Socket.
		 */
		int socketSend(char *buffer, int size);
		int socketSend(const char *str);

		/*
		 * Receive UDP/TCP Socket.
		 */
		int socketRecv(char *buffer, int bufferSize, int *recvSize);

		/*
		 * Power Saving Mode Disable function. TYPE1SC Default, 0(Disable)
		 */
		int disablePSM(void);

		/*
		 * Reset the module.
		 */
		void reset(void);

		/*
		 * check SIM status.
		 */
		int chkSIM(void);

	private:
		int sendATcmdOmitOK(char *szCmd, char *szResponse, int nResponseBufSize,
				const char *szResponseFilter,
				unsigned long ulWaitDelay = 10000);

		int sendATcmdOmitOK(char *szCmd, char *szResponse, int nResponseBufSize,
				const char *szResponseFilter, char *SendData,
				int SendDataSize, unsigned long ulWaitDelay = 10000);

		int writeATcmd(char *szCmd, char *szResponse, int nResponseBufSize,
				const char *szResponseFilter,
				unsigned long ulWaitDelay = 2000);

		int sendATcmd(char *szCmd, char *szResponse, int nResponseBufSize,
				const char *szResponseFilter, unsigned long ulWaitDelay = 2000);

		int sendATcmd(char *szCmd, char *aLine[], int nMaxLine,
				unsigned long ulWaitDelay = 2000);

		int readATresponseLineOmitOK(char *szLine, int nLineBufSize,
				const char *szResponseFilter,
				unsigned long ulDelay);

		int readATresponseLine(char *szLine, int nLineBufSize,
				const char *szResponseFilter, unsigned long ulDelay);

		int readATresponseHTTPLine(char *szLine, int nLineBufSize,
				const char *szResponseFilter, unsigned long ulDelay);

		int readATresponseLine(char *aLine[], int nMaxLine, unsigned long ulDelay);

		void TYPE1SC_serial_clearbuf(void);

		void TYPE1SC_trace(const __FlashStringHelper *szTrace, ...);

		Stream &_serial;
		Stream &_debug;
		uint8_t _pwr_pin;
		uint8_t _reset_pin;
		uint8_t _wakeup_pin;
		uint8_t _extAnt_pin;
		int _timeOut = 0;
		int _nSocket = 1;
};
#endif
