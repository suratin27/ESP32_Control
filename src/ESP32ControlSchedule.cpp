#include <Arduino.h>
#include "ESP32ControlSchedule.h" 

//#include "TimeAlarms.h"
#include <time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "ESP32Time.h" 

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
ESP32Time rtc;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 3600;
_callbackData callBackData;
unsigned long lastRTCUpdate = 0;
unsigned long peroid  = 0;

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S"); /*
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();*/
}

time_t getRTCTime(){
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    delay(1000);
  }
  rtc.setTimeStruct(timeinfo);
  Serial.println("Realtime sync from NTP Server already."); 

  return rtc.getEpoch();
}

void updateRTCTime(){
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    delay(1000);
  }
  rtc.setTimeStruct(timeinfo);
  Serial.println("Realtime sync from NTP Server already."); 

}

void initNTPTime(unsigned long _syncPeroid){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    delay(1000);
  }
  rtc.setTimeStruct(timeinfo);
  Serial.println("Realtime sync already."); 

  peroid = _syncPeroid;

  //setSyncProvider(getRTCTime);
  //setSyncInterval(100);
}

void printTimeStamp(){
  Serial.print("RTC Time stamp: ");
  Serial.println(rtc.getEpoch());
}

void getESP32ControlTime(struct tm* _t){
  *_t = rtc.getTimeStruct();
}

tfRoutine::tfRoutine(tm* _tm_struct){
  myTime = _tm_struct;
}

bool tfRoutine::registerRoutine(uint8_t _id,uint8_t _onHour,uint8_t _onMin,uint8_t _offHour,uint8_t _offMin,OnTick_t _tickFunc){
  if(!routine[_id].allocated){
    routine[_id].onHour   = _onHour;
    routine[_id].onMin    = _onMin;
    routine[_id].offHour  = _offHour;
    routine[_id].offMin   = _offMin;
    routine[_id].allocated  = true;
    routine[_id].tickFunc = _tickFunc;

    return true;
  }else{
    return false;
  }
}

void tfRoutine::Tick(){
  if(millis() - lastRTCUpdate > peroid){
  lastRTCUpdate = millis();
    updateRTCTime();
  }
  for(uint8_t id=0;id<MAX_ROUTINE;id++){
    if(routine[id].allocated){
      if(routine[id].onHour >= routine[id].offHour){         //- On Hour > Off Hour
        if(myTime->tm_hour > routine[id].onHour){           //- ชม. มากกว่า On Hour 
          routine[id].currH = myTime->tm_hour;
          routine[id].currM = myTime->tm_min;
          routine[id].status = true;  //1;
        }else if(myTime->tm_hour == routine[id].onHour){    //- ชม. On Hour เท่ากัน แต่นาทีมากกว่า
          if((myTime->tm_min >= routine[id].onMin)&&(myTime->tm_min < routine[id].offMin)){   //ถ้าชม. เท่ากัน แต่อยู่ในช่วงนาที onMin
            routine[id].currH = myTime->tm_hour;
            routine[id].currM = myTime->tm_min;
            routine[id].status = true;   //11;
          }else{
            routine[id].currH = myTime->tm_hour;
            routine[id].currM = myTime->tm_min;
            routine[id].status = false;   //22;    //- Off Min less
          }
        }else if(myTime->tm_hour > routine[id].offHour){    //- ชม. มากกว่า Off Hour 
          routine[id].currH = myTime->tm_hour;
          routine[id].currM = myTime->tm_min;
          routine[id].status = false;   //2;
        }else if(myTime->tm_hour == routine[id].offHour){   //- ชม. Off Hour เท่ากัน แต่นาทีมากกว่า
          if(myTime->tm_min >= routine[id].offMin){
            routine[id].currH = myTime->tm_hour;
            routine[id].currM = myTime->tm_min;
            routine[id].status = false;   //21;
          }else{
            routine[id].currH = myTime->tm_hour;
            routine[id].currM = myTime->tm_min;
            routine[id].status = true;    //12;                        //- On Min less
          }
        }else{
          routine[id].currH = myTime->tm_hour;
          routine[id].currM = myTime->tm_min;
          routine[id].status = true;   //3;
        }
      }else{                                                //- On Hour < Off Hour 
        if(myTime->tm_hour > routine[id].offHour){          //- ชม. มากกว่า Off Hour 
          routine[id].currH = myTime->tm_hour;
          routine[id].currM = myTime->tm_min;
          routine[id].status = false;   //101;
        }else if(myTime->tm_hour == routine[id].offHour){   //- ชม. Off Hour เท่ากัน แต่นาทีมากกว่า
          if(myTime->tm_min >= routine[id].offMin){
            routine[id].currH = myTime->tm_hour;
            routine[id].currM = myTime->tm_min;
            routine[id].status = false;   //111;
          }else{
            routine[id].currH = myTime->tm_hour;
            routine[id].currM = myTime->tm_min;
            routine[id].status = true;   //122;                      //- On Min less
          }
        }else if(myTime->tm_hour > routine[id].onHour){    //- ชม. มากกว่า On Hour 
          routine[id].currH = myTime->tm_hour;
          routine[id].currM = myTime->tm_min;
          routine[id].status = true;   //102;
        }else if(myTime->tm_hour == routine[id].onHour){   //- ชม. On Hour เท่ากัน แต่นาทีมากกว่า
          if(myTime->tm_min >= routine[id].onMin){
            routine[id].currH = myTime->tm_hour;
            routine[id].currM = myTime->tm_min;
            routine[id].status = true;   //121;
          }else{
            routine[id].currH = myTime->tm_hour;
            routine[id].currM = myTime->tm_min;
            routine[id].status = false;   //112;                      //- Off Min less
          }
        }else{
          routine[id].currH = myTime->tm_hour;
          routine[id].currM = myTime->tm_min;
          routine[id].status = false;   //103;
        }
      }
      
      callBackData.day  = myTime->tm_wday;
      callBackData.hour = myTime->tm_hour;
      callBackData.min  = myTime->tm_min;
      callBackData.sec  = myTime->tm_sec;
      callBackData.status = routine[id].status;
      //routine[id].tickFunc(routine[id].status);
      routine[id].tickFunc(callBackData);
    }
  } 
}