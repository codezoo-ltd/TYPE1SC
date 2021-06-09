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
		TYPE1SC(Stream &serial, Stream &debug, uint8_t pwr_pin, uint8_t status_pin);

		/*
		 * Power on the module.
		 */
		int pwrON(void);

		/*
		 * Power off the module.
		 */
		int pwrOFF(void);

		/*
		 * Power status check the module.
		 */
		int isPwrON(void);

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
		 * Request Change device functionality status. (0-1)
		 */
		int getCFUN(int *value);

		/*
		 * Set Change device functionality. (0-1)
		 */
		int setCFUN(int value);

		/*
		 * Request international mobile subscriber identity.
		 */
		int getCIMI(char *szCIMI, int nBufferSize);

		/*
		 * PDP status check the module.
		 */
		int isActPDP(char *szPDP, int nBufferSize);

		/*
		 * Activate a PDP Context
		 */
		int actPDP(void);

		/*
		 * Deactivate a PDP Context
		 */
		int deActPDP(void);

		/*
		 * EPS network registration status.
		 */
		int canConnect(void);

		/*
		 * Get serving cellID information.
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
		//	void TYPE1SC_reset(void);

	private:
		int sendATcmd(char *szCmd, char *szResponse, int nResponseBufSize,
				const char *szResponseFilter, unsigned long ulWaitDelay = 2000);

		int sendATcmd(char *szCmd, char *aLine[], int nMaxLine,
				unsigned long ulWaitDelay = 2000);

		int readATresponseLine(char *szLine, int nLineBufSize,
				const char *szResponseFilter, unsigned long ulDelay);

		int readATresponseLine(char *aLine[], int nMaxLine, unsigned long ulDelay);

		void TYPE1SC_serial_clearbuf(void);

		void TYPE1SC_trace(const __FlashStringHelper *szTrace, ...);

		Stream &_serial;
		Stream &_debug;
		uint8_t _pwr_pin;
		uint8_t _status_pin;
		int _timeOut = 0;
		int _nSocket = 1;
};
#endif
