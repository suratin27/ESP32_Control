
#ifndef ESP32CONTROLIO_H
#define ESP32CONTROLIO_H

#include <Arduino.h>
/*---------------------------------------------------------------------
                    define I/O for ESP Control board
---------------------------------------------------------------------*/

const uint8_t WOR_LED = 5;
const uint8_t Ix10[8] = {39,36,22,23,21,19,18,4};
const uint8_t Qx10[8] = {13,14,27,26,25,33,32,2};
const uint8_t Ix10RS[8] = {39,36,35,34,21,19,18,4};
const uint8_t Qx10RS[8] = {13,14,27,26,25,33,32,2};

//-Set board version
void setBoardversion(uint8_t v);

uint8_t getBoardVersion();

//parameter: board version ,0 for v1.0RS,v1.0RX,RX-32U and 1.0RX ,1 for v1.0
void initIO(uint8_t _io = 0);

//parameter: none
void setLed();

//parameter: node
void resetLed();

//parameter: node
void toggleLed();

//parameter: prefered input ,return bool
bool readInput(uint8_t _i);

//parameter: prefered output ,return bool
bool getOutput(uint8_t _i);

//parameter: prefered output
void setOutput(uint8_t _o);

//parameter: prefered output
void resetOutput(uint8_t _o);

//parameter: prefered output
void toggleOutput(uint8_t _o);

//parameter: none 
void clearAllOutput();

//parameter: node
void setAllOutput();

//parameter: Status led
void toggleSTS(uint8_t _led);

//parameter: none
void clearSTS();

#endif