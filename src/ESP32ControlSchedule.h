#ifndef ESP32CONTROLSCHEDULE_H
#define ESP32CONTROLSCHEDULE_H

  struct _callbackData{
    int day;
    int hour;
    int min;
    int sec;
    bool status;
  };

  typedef void (*OnTick_t)(_callbackData);
  typedef tm (*tm_struct)();

  #define MAX_ROUTINE   32

  struct Routine{
    int ID;
    int onHour;
    int onMin;
    int offHour;
    int offMin;
    int currH;
    int currM;
    bool status;
    //int status;
    bool allocated;
    OnTick_t tickFunc;
  };

  void printLocalTime();
  time_t getRTCTime();
  void updateRTCTime();
  //--------------- (unsigned long sync time peroid (ms))
  void initNTPTime(unsigned long _syncPeroid = 10000);
  void printTimeStamp();
  void getESP32ControlTime(struct tm* _t);
  void updateRTCTime();

  class tfRoutine{
    public:
      tfRoutine(tm* _tm_struct);
      //-(id,onTimeHour,onTimeMin,offTimeHour,offTimeMin,tickFunction)
      bool registerRoutine(uint8_t _id,uint8_t _onHour,uint8_t _onMin,uint8_t _offHour,uint8_t _offMin,OnTick_t _tickFunc);
      void Tick();
    private:
      Routine routine[MAX_ROUTINE];
      tm* myTime;
  };
  
#endif