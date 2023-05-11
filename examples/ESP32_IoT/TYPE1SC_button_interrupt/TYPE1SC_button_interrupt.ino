#include <Arduino.h>
#include <algorithm>

#define SW_01        26
#define BATT_ADC     35
#define AVGCNT       30

int btn1 = 0;
unsigned long button_time1 = 0;  
unsigned long last_button_time1 = 0;

//Pressed for more than 300ms
void IRAM_ATTR SW01_ISR(){
	button_time1 = millis();
	if (button_time1 - last_button_time1 > 300)
	{
		btn1 = 1;
		last_button_time1 = button_time1;                
	}  
}

float readBattery()
{
	uint8_t samples = AVGCNT;
	float   array[AVGCNT];
	float   batt_adc = 0;

	for (int i = 0; i < samples; i++) {
		int vref = 3300;
		uint16_t volt = analogRead(BATT_ADC);
		float battery_voltage = ((float)volt / 4095.0) * (vref) * 2;

		array[i] = battery_voltage;
		delay(10);
	}

	std::sort(array, array + samples);
	for (int i = 0; i < samples; i++) {
		if (i == 0 || i == samples - 1)
			continue;
		batt_adc += array[i];
	}

	batt_adc /= samples - 2;
	return batt_adc;
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);
	delay(1000);
	Serial.println("hello button");
	pinMode(SW_01, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(SW_01), SW01_ISR, FALLING);
}

void loop() {
	// put your main code here, to run repeatedly:
	if(btn1 == 1)   //SW1 interrupt
	{
		float battery_voltage = readBattery(); 
		btn1 = 0;   
		Serial.println("Switch_1 Pressed");
		Serial.print("BATTERY : ");
		Serial.println(battery_voltage);
	}
}
