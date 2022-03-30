
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

void initIO(uint8_t _io = 0);
void setLed();
void resetLed();
void toggleLed();
bool readInput(uint8_t _i);
bool getOutput(uint8_t _i);
void setOutput(uint8_t _o);
void resetOutput(uint8_t _o);
void toggleOutput(uint8_t _o);
void clearAllOutput();
void setAllOutput();

#endif