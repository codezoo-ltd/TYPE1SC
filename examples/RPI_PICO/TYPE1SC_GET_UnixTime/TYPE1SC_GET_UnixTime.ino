#include <UnixTime.h> //https://github.com/GyverLibs/UnixTime
#include "TYPE1SC.h"

#define DebugSerial Serial
#define M1Serial Serial1 // RPI_PICO

#define PWR_PIN 4
#define RST_PIN 5
#define WAKEUP_PIN 6

UnixTime stamp(9);  //Seoul GMT + 09
TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // put your setup code here, to run once:
  M1Serial.begin(115200);
  DebugSerial.begin(115200);

  DebugSerial.println("TYPE1SC Module Start!!!");

  /* Board Reset(Using ATCmd) */
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
}

void loop() {
  /* Get Time (GMT, (+36/4) ==> Korea +9hour) */
  char szTime[32];
  uint32_t _year,_month, _day, _hour, _minute, _second, _tmp;
   
  if (TYPE1SC.getCCLK(szTime, sizeof(szTime)) == 0) {
    DebugSerial.print("Time : ");
    DebugSerial.println(szTime);
    sscanf(szTime,"\"%d/%d/%d,%d:%d:%d+%d\"",&_year, &_month, &_day, &_hour, &_minute, &_second, &_tmp);
    /* Debug */    
    DebugSerial.println(_year);
    DebugSerial.println(_month);
    DebugSerial.println(_day);
    DebugSerial.println(_hour);
    DebugSerial.println(_minute);
    DebugSerial.println(_second);
  }    
  //Set Date Time
  _year += 2000;
  stamp.setDateTime(_year, _month, _day, _hour, _minute, _second);

  //Get Unix Time 
  uint32_t unix = stamp.getUnix();
  DebugSerial.println(unix);

  // ========== UNIX to date and time ===========
  // convert unix to date and time
  // getDateTime(unix stamp) runs ~500 us on AVR
  // https://www.unixtimestamp.com/index.php
  stamp.getDateTime(unix);
    
// pick up like this
  DebugSerial.println(stamp.year);
  DebugSerial.println(stamp.month);
  DebugSerial.println(stamp.day);
  DebugSerial.println(stamp.hour);
  DebugSerial.println(stamp.minute);
  DebugSerial.println(stamp.second);

  delay(1000);    
}
