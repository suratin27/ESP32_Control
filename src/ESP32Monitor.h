#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include "SPIFFS.h"
#include "ESP32ControlIO.h"

String getValueReadings();
void initSPIFFS();
void setupWebserver();
void webMonTask();

//- Update memory M
void defMName(bool setDiag = false);
void setMName(uint8_t sAdd,String sn,bool setDiag = false);
void updateMValue(uint8_t add, bool val,bool setDiag = false);
void clearMValue(bool setDiag = false);
//- Update memory D
void defDName(bool setDiag = false);
void setDName(uint8_t sAdd,String sn,bool setDiag = false);
void updateDValue(uint8_t add, uint16_t val,bool setDiag = false);
void clearDValue(bool setDiag = false);
//- Update memory F
void defFName(bool setDiag = false);
void setFName(uint8_t sAdd,String sn,bool setDiag = false);
void updateFValue(uint8_t add, float val,bool setDiag = false);
void clearFValue(bool setDiag = false);
//- Prompt msg
void promptMsg();
//- Process CMD
void processCMD();
//- Fetch command 
void fetchCMD();
//- Add Diagnostic
void addDiagnostic(String _diag);
//- Memory definitions
void defMemory(bool setDiag = false);
//- Init ESP32 Monitor
void initESP32Monitor(uint8_t _boardVersion = 0);