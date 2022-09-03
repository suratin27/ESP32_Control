#include "ESP32ControlIO.h"

uint8_t myIO = 0;

void initIO(uint8_t _io){
  myIO = _io;
  if(_io == 0){
    Serial.println("--------------------------------.");
    Serial.println(" Module model: ESP32 Control v1.0RS");
    Serial.println("--------------------------------.");
    Serial.println("Init Input pin.");
      for(uint8_t i=0;i<8;i++){ //Init I output
        pinMode(Ix10RS[i],INPUT);
      }
    Serial.println("Init - OK.");
    Serial.println("-----------------------.");
    Serial.println("Init Output pin.");
      for(uint8_t i=0;i<8;i++){ //Init Q output and set to Low
        pinMode(Qx10RS[i],OUTPUT);
        digitalWrite(Qx10RS[i],LOW);
      }
  }else if(_io == 1){
    Serial.println("--------------------------------.");
    Serial.println(" Module model: ESP32 Control v1.0RX");
    Serial.println("--------------------------------.");
    Serial.println("Init Input pin.");
      for(uint8_t i=0;i<8;i++){ //Init I output
        pinMode(Ix10RS[i],INPUT);
      }
    Serial.println("Init - OK.");
    Serial.println("-----------------------.");
    Serial.println("Init Output pin.");
      for(uint8_t i=0;i<8;i++){ //Init Q output and set to Low
        pinMode(Qx10RS[i],OUTPUT);
        digitalWrite(Qx10RS[i],LOW);
      }
  }else if(_io == 2){
    Serial.println("--------------------------------.");
    Serial.println(" Module model: MINIPLC RLX 32U");
    Serial.println("--------------------------------.");
    Serial.println("Init Input pin.");
      for(uint8_t i=0;i<8;i++){ //Init I output
        pinMode(Ix10RS[i],INPUT);
      }
    Serial.println("Init - OK.");
    Serial.println("-----------------------.");
    Serial.println("Init Output pin.");
      for(uint8_t i=0;i<8;i++){ //Init Q output and set to Low
        pinMode(Qx10RS[i],OUTPUT);
        digitalWrite(Qx10RS[i],LOW);
      }
  }else if(_io == 3){
    Serial.println("--------------------------------.");
    Serial.println(" Module model: LAGACY MODEL");
    Serial.println("--------------------------------.");
    Serial.println("Init Input pin.");
      for(uint8_t i=0;i<8;i++){ //Init I output
        pinMode(Ix10[i],INPUT);
      }
    Serial.println("Init - OK.");
    Serial.println("-----------------------.");
    Serial.println("Init Output pin.");
      for(uint8_t i=0;i<8;i++){ //Init Q output and set to Low
        pinMode(Qx10[i],OUTPUT);
        digitalWrite(Qx10[i],LOW);
      }
  }
     
  Serial.println("-----------------------.");
  Serial.println("Init Working LED Status.");
  pinMode(WOR_LED,OUTPUT);
  
  Serial.println("Init - OK.");
  Serial.println("-----------------------.");
}

void setLed(){
  digitalWrite(WOR_LED,HIGH);
}

void resetLed(){
  digitalWrite(WOR_LED,LOW);
}

void toggleLed(){
  digitalWrite(WOR_LED, !digitalRead(WOR_LED));
}

bool readInput(uint8_t _i){
  if(myIO == 0){
    return !digitalRead(Ix10RS[_i]);
  }else{
    return !digitalRead(Ix10[_i]);
  }
}

bool getOutput(uint8_t _i){
  if(myIO == 0){
    return digitalRead(Qx10RS[_i]);
  }else{
    return digitalRead(Qx10[_i]);
  }
}

void setOutput(uint8_t _o){
  if(myIO == 0){
    digitalWrite(Qx10RS[_o], HIGH);
  }else{
    digitalWrite(Qx10[_o], HIGH);
  }
}

void resetOutput(uint8_t _o){
  if(myIO == 0){
    digitalWrite(Qx10RS[_o], LOW);
  }else{
    digitalWrite(Qx10[_o], LOW);
  }
}

void toggleOutput(uint8_t _o){
  if(myIO == 0){
    digitalWrite(Qx10RS[_o], !digitalRead(Qx10RS[_o]));
  }else{
    digitalWrite(Qx10[_o], !digitalRead(Qx10[_o]));
  }
}

void clearAllOutput(){
  for(uint8_t i=0;i<8;i++){
    resetOutput(i);
  }
}

void setAllOutput(){
  for(uint8_t i=0;i<8;i++){
    setOutput(i);
  }
}

void toggleSTS(uint8_t _led){
  if(_led == 0){
    toggleOutput(6);
  }else if(_led == 1){
    toggleOutput(5);
  }else if(_led == 2){
    toggleOutput(4);
  }else if(_led == 3){
    toggleOutput(7);
  }
}

void clearSTS(){
  resetOutput(4);
  resetOutput(5);
  resetOutput(6);
  resetOutput(7);
}