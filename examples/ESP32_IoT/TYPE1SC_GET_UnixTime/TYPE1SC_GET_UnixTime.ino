#include "TYPE1SC.h"
#include <Arduino.h>
#include <U8x8lib.h>
#include <UnixTime.h> //https://github.com/GyverLibs/UnixTime

#define DebugSerial Serial
#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19
#define EXT_ANT 4

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);
HardwareSerial M1Serial(2); // use ESP32 UART2
TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);
UnixTime stamp(9); // Seoul GMT + 09

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
  u8x8log.setRedrawMode(
      1); // 0: Update screen with newline, 1: Update screen for every char

  // put your setup code here, to run once:
  /* Serial2 Initialization */
  M1Serial.begin(115200, SERIAL_8N1, 16, 17); // RXD2 : 16, TXD2 : 17
  DebugSerial.begin(115200);

  DebugSerial.println("TYPE1SC Module Start!!!");
  u8x8log.print("TYPE1SC Module Start!!!\n");

  extAntenna();

  /* TYPE1SC Module Initialization */
  if (TYPE1SC.init()) {
    DebugSerial.println("TYPE1SC Module Error!!!");
    u8x8log.print("TYPE1SC Module Error!!!\n");
  }

  /* Network Regsistraiton Check */
  while (TYPE1SC.canConnect() != 0) {
    DebugSerial.println("Network not Ready !!!");
    u8x8log.print("Network not Ready!!!\n");
    delay(2000);
  }

  DebugSerial.println("TYPE1SC Module Ready!!!");
  u8x8log.print("TYPE1SC Module Ready!!!\n");
}

void loop() {
  /* Get Time (GMT, (+36/4) ==> Korea +9hour) */
  char szTime[32];
  uint32_t _year, _month, _day, _hour, _minute, _second, _tmp;

  if (TYPE1SC.getCCLK(szTime, sizeof(szTime)) == 0) {
    DebugSerial.print("Time : ");
    DebugSerial.println(szTime);
    u8x8log.print("Time : ");
    u8x8log.print(szTime);
    u8x8log.print("\n");
    sscanf(szTime, "\"%d/%d/%d,%d:%d:%d+%d\"", &_year, &_month, &_day, &_hour,
           &_minute, &_second, &_tmp);
    /* Debug */
    DebugSerial.println(_year);
    u8x8log.print(_year);
    u8x8log.print("\n");
    DebugSerial.println(_month);
    u8x8log.print(_month);
    u8x8log.print("\n");
    DebugSerial.println(_day);
    u8x8log.print(_day);
    u8x8log.print("\n");
    DebugSerial.println(_hour);
    u8x8log.print(_hour);
    u8x8log.print("\n");
    DebugSerial.println(_minute);
    u8x8log.print(_minute);
    u8x8log.print("\n");
    DebugSerial.println(_second);
    u8x8log.print(_second);
    u8x8log.print("\n");
  }
  // Set Date Time
  _year += 2000;
  stamp.setDateTime(_year, _month, _day, _hour, _minute, _second);

  // Get Unix Time
  uint32_t unix = stamp.getUnix();
  DebugSerial.println(unix);
  u8x8log.print(unix);
  u8x8log.print("\n");

  // ========== UNIX to date and time ===========
  // convert unix to date and time
  // getDateTime(unix stamp) runs ~500 us on AVR
  // https://www.unixtimestamp.com/index.php
  stamp.getDateTime(unix);

  // pick up like this
  DebugSerial.println(stamp.year);
  u8x8log.print(stamp.year);
  u8x8log.print("\n");
  DebugSerial.println(stamp.month);
  u8x8log.print(stamp.month);
  u8x8log.print("\n");
  DebugSerial.println(stamp.day);
  u8x8log.print(stamp.day);
  u8x8log.print("\n");
  DebugSerial.println(stamp.hour);
  u8x8log.print(stamp.hour);
  u8x8log.print("\n");
  DebugSerial.println(stamp.minute);
  u8x8log.print(stamp.minute);
  u8x8log.print("\n");
  DebugSerial.println(stamp.second);
  u8x8log.print(stamp.second);
  u8x8log.print("\n");

  delay(1000);
}
