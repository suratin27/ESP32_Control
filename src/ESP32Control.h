#ifndef ESP32CONTROL_H
#define ESP32CONTROL_H


#if defined(ESP32_CONTROL_10RS)
  #define BOARD_TXT     "ESP32 Control - RS"
  #define BOARD_VERSION  0
#elif defined(ESP32_CONTROL_10RX)
  #define BOARD_TXT     "ESP32 Control - RX"
  #define BOARD_VERSION  1
#elif defined(MINIPLC_RLX_32U)
  #define BOARD_TXT     "MINIPLC RLX 32U"
  #define BOARD_VERSION  2
#else
  #define BOARD_TXT     "ESP32 Control - Lagacy"
  #define BOARD_VERSION  3
#endif

#if defined(ESP32_CONTROL_10RS) || defined(ESP32_CONTROL_10RX) || defined(MINIPLC_RLX_32U)
  #include "ModbusSlaveSerial.h"
  #include "ModbusMaster232.h"
#endif
#if defined(ESP32_CONTROL_10) || defined(ESP32_CONTROL_10RS) || defined(ESP32_CONTROL_10RX) || defined(MINIPLC_RLX_32U)
  #include "ESP32ControlIO.h"
  #include "modbusTTCP.h"
  #include "ESP32ControlSchedule.h"
  #include "ModbusIP_ESP8266.h"
  #include "ESP32Monitor.h"
#endif


#endif