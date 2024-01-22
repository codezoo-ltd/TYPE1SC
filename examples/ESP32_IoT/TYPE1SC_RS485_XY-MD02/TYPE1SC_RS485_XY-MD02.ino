/*
   This code demonstrates how to interact with an Arduino Mega 2560 and
   a Modbus RTU temperature and humidity sensor (SHT20). It reads the
   temperature and humidity values every 1 seconds and display data to
   the serial monitor.

Note: Serial Port 0 is not used to connect the RS485 Converter (MAX485)
because its used for debugging. The Serial Port 1 (TX1, RX1) is used
for ModBus communication interface.

Wiring of Sensor, Arduino, and MAX485 TTL to RS485 Converter:
___________________________________________________________________________________________
| Sensor (SHT20)   |   MAX485 TTL to RS485 Converter
|  A (Yellow)      |        A (Terminal block)
|  B (White)       |        B (Terminal block)
|  GND (Black)     |       GND (External Supply)
|  Vs (Red)        |      9-30V (External Supply)
___________________________________________________________________________________________
| MAX485 TTL to RS485 Converter  |  Arduino (Hardware Serial)  |  Arduino (Software Serial)
|     RO (Reciever Output)       |        D19 (RX1)            |          D9 (RX)
|     RE (Reciever Enable)       |        D2                   |          D2
|     DE (Driver Enable)         |        D3                   |          D3
|     DI (Driver Input)          |        D18 (TX1)            |          D10 (TX)
___________________________________________________________________________________________
 */
#include <Arduino.h>
#include <HardwareSerial.h>
#include <ModbusMaster.h>

#define RS485Serial SerialPort

HardwareSerial SerialPort(1); // use ESP32 UART1
ModbusMaster node;

void setup() {

	/* Serial1 Initialization */
	SerialPort.begin(9600, SERIAL_8N1, 33, 32); // RXD1 : 33, TXD1 : 32

	// Modbus communication runs at 9600 baud
	Serial.begin(115200);

	// Modbus slave ID 1
	node.begin(1, RS485Serial);

	// Callbacks allow us to configure the RS485 transceiver correctly
	// Auto FlowControl - NULL
	node.preTransmission(NULL);
	node.postTransmission(NULL);

	delay(2000);
}

void loop() {
	// Request 2 registers starting at 0x0001
	uint8_t result = node.readInputRegisters(0x0001, 2);
	Serial.println("Data Requested");

	if (result == node.ku8MBSuccess) {
		// Get response data from sensor
		Serial.print("Temperature: ");
		Serial.print(float(node.getResponseBuffer(0) / 10.00F));
		Serial.print("   Humidity: ");
		Serial.println(float(node.getResponseBuffer(1) / 10.00F));
	}
	delay(2000);
}
