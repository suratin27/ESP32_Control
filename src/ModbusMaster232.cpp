/**
@file
Arduino library for communicating with Modbus slaves over RS232/485 (via RTU protocol).
*/
/*

  ModbusMaster.cpp - Arduino library for communicating with Modbus slaves
  over RS232/485 (via RTU protocol).
  
  This file is part of ModbusMaster.
  
  ModbusMaster is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  ModbusMaster is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with ModbusMaster.  If not, see <http://www.gnu.org/licenses/>.
  
  Written by Doc Walker (Rx)
  Copyright © 2009-2013 Doc Walker <4-20ma at wvfans dot net>
  
  
 Modified by PDAControl 2016
 - Function  crc16 
 - Function  makeWord
   
  
*/
#include "ModbusMaster232.h"
//#include <SoftwareSerial.h>  // Modbus RTU pins   D7(13),D8(15)   RX,TX

#include "Arduino.h"

//SoftwareSerial swSer(7, 8, false, 256);
//#define ESP-01
#define ESP32_CPU

#ifdef ESP32_CPU
  HardwareSerial *port;
#endif


/* _____PUBLIC FUNCTIONS_____________________________________________________ */

/**
Constructor.
Creates class object using default serial port 0, Modbus slave ID 1.
@ingroup setup
*/

#ifndef ESP32_CPU
HardwareSerial *port;
ModbusMaster232::ModbusMaster232(uint8_t u8MBSlave){
  _u8SerialPort = 0;
  _u8MBSlave = 1;
}
#endif

//Null callback
void callback_func_null(uint8_t func, uint16_t dAdd, bool iBit ,uint16_t iVal ,float fVal){

}

ModbusMaster232::ModbusMaster232(void){
  _u8SerialPort = 0;
  port = &Serial;
  _u8MBSlave = 1;
  setCallback(callback_func_null);
}

ModbusMaster232::ModbusMaster232(uint8_t _serialNo){
  if(_serialNo==0){
    port = &Serial;
  }
  else if(_serialNo==1){
    port = &Serial1;
  }
  else if(_serialNo==2){
   port = &Serial2;
   _u8SerialPort = 2;
  }
  
  _u8MBSlave = 1;
  setCallback(callback_func_null);
}

ModbusMaster232::ModbusMaster232(uint8_t _serialNo,uint8_t u8MBSlave){
  if(_serialNo==0){
    port = &Serial;
  }
  else if(_serialNo==1){
    port = &Serial1;
  }
  _u8SerialPort = 0;
  _u8MBSlave = u8MBSlave;
  setCallback(callback_func_null);
}

ModbusMaster232::ModbusMaster232(uint8_t _serialNo,unsigned long _timeout,uint8_t _tryCount){
  if(_serialNo==0){
    port = &Serial;
  }
  else if(_serialNo==1){
    port = &Serial1;
  }
  
  if(_timeout<10){
    timeout = _timeout;
  }else{
    timeout = MB_TIMEOUT;
  }
  
  if(_tryCount<1){
    tryCount = _tryCount;
  }else{
    tryCount = MB_RETRY;
  }
  
  _u8SerialPort = 0;
  _u8MBSlave = 1;
  setCallback(callback_func_null);
}


/**
CRC ESP8266

Creates class object using default serial port 0, specified Modbus slave ID.

@overload void ModbusMaster::ModbusMaster(uint8_t u8MBSlave)
@param u8MBSlave Modbus slave ID (1..255)
@ingroup setup
*/

//Function  crc16 created for ESP8266   - PDAControl
//http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__util__crc_1ga95371c87f25b0a2497d9cba13190847f.html
// append CRC
  
uint16_t _crc16_update2 (uint16_t crc, uint8_t a){
    int i;

    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }

    return crc;
}

/**
Initialize class object.

Sets up the serial port using default 19200 baud rate.
Call once class has been instantiated, typically within setup().

@ingroup setup
*/
#ifndef ESP32_CPU
void ModbusMaster232::begine(unsigned long Baudrate){
 Serial.begin(Baudrate);
}
#endif

void ModbusMaster232::begin(void){
  //swSer.begin(9600);
  port->begin(9600);
}

/**
Initialize class object.

Sets up the swSer port using specified baud rate.
Call once class has been instantiated, typically within setup().

@overload ModbusMaster::begin(uint16_t u16BaudRate)
@param u16BaudRate baud rate, in standard increments (300..115200)
@ingroup setup
*/
void ModbusMaster232::begin(unsigned long BaudRate ){
//  txBuffer = (uint16_t*) calloc(ku8MaxBufferSize, sizeof(uint16_t));
  _u8TransmitBufferIndex = 0;
  u16TransmitBufferLength = 0;

  delay(100);  
  if(_u8SerialPort==2){
   port->begin(BaudRate,SERIAL_8N1, 16, 17);
  }else{
   port->begin(BaudRate);
  }
  
  //.begin(BaudRate);
}

void ModbusMaster232::beginTransmission(uint16_t u16Address){
  _u16WriteAddress = u16Address;
  _u8TransmitBufferIndex = 0;
  u16TransmitBufferLength = 0;
}

// eliminate this function in favor of using existing MB request functions
uint8_t ModbusMaster232::requestFrom(uint16_t address, uint16_t quantity){
  uint8_t read;
  // clamp to buffer length
  if(quantity > ku8MaxBufferSize)
  {
    quantity = ku8MaxBufferSize;
  }
  // set rx buffer iterator vars
  _u8ResponseBufferIndex = 0;
  _u8ResponseBufferLength = read;

  return read;
}

void ModbusMaster232::sendBit(bool data){
  uint8_t txBitIndex = u16TransmitBufferLength % 16;
  if ((u16TransmitBufferLength >> 4) < ku8MaxBufferSize)
  {
    if (0 == txBitIndex)
    {
      _u16TransmitBuffer[_u8TransmitBufferIndex] = 0;
    }
    bitWrite(_u16TransmitBuffer[_u8TransmitBufferIndex], txBitIndex, data);
    u16TransmitBufferLength++;
    _u8TransmitBufferIndex = u16TransmitBufferLength >> 4;
  }
}

void ModbusMaster232::send(uint16_t data){
  if (_u8TransmitBufferIndex < ku8MaxBufferSize)
  {
    _u16TransmitBuffer[_u8TransmitBufferIndex++] = data;
    u16TransmitBufferLength = _u8TransmitBufferIndex << 4;
  }
}

void ModbusMaster232::send(uint32_t data){
  send(lowWord(data));
  send(highWord(data));
}

void ModbusMaster232::send(uint8_t data){
  send(uint16_t(data));
}

uint8_t ModbusMaster232::available(void){
  return _u8ResponseBufferLength - _u8ResponseBufferIndex;
}

uint16_t ModbusMaster232::receive(void){
  if (_u8ResponseBufferIndex < _u8ResponseBufferLength)
  {
    return _u16ResponseBuffer[_u8ResponseBufferIndex++];
  }
  else
  {
    return 0xFFFF;
  }
}


void ModbusMaster232::setConfig(uint8_t _serialNo,unsigned long _timeout,uint8_t _tryCount){
  if(_serialNo==0){
    port = &Serial;
  }
  else if(_serialNo==1){
    port = &Serial1;
  }
  
  if(_timeout<10){
    timeout = _timeout;
  }else{
    timeout = MB_TIMEOUT;
  }
  
  if(_tryCount<1){
    tryCount = _tryCount;
  }else{
    tryCount = MB_RETRY;
  }
  
  _u8SerialPort = 0;
  _u8MBSlave = 1;
}


/**
Set idle time callback function (cooperative multitasking).

This function gets called in the idle time between transmission of data
and response from slave. Do not call functions that read from the serial
buffer that is used by ModbusMaster. Use of i2c/TWI, 1-Wire, other
serial ports, etc. is permitted within callback function.

@see ModbusMaster::ModbusMasterTransaction()
*/
void ModbusMaster232::idle(void (*idle)()){
  _idle = idle;
}


/**
Retrieve data from response buffer.

@see ModbusMaster::clearResponseBuffer()
@param u8Index index of response buffer array (0x00..0x3F)
@return value in position u8Index of response buffer (0x0000..0xFFFF)
@ingroup buffer
*/
uint16_t ModbusMaster232::getResponseBuffer(uint8_t u8Index){
  if (u8Index < ku8MaxBufferSize)
  {
    return _u16ResponseBuffer[u8Index];
  }
  else
  {
    return 0xFFFF;
  }
}


/**
Clear Modbus response buffer.

@see ModbusMaster::getResponseBuffer(uint8_t u8Index)
@ingroup buffer
*/
void ModbusMaster232::clearResponseBuffer(){
  uint8_t i;
  
  for (i = 0; i < ku8MaxBufferSize; i++)
  {
    _u16ResponseBuffer[i] = 0;
  }
}


/**
Place data in transmit buffer.

@see ModbusMaster::clearTransmitBuffer()
@param u8Index index of transmit buffer array (0x00..0x3F)
@param u16Value value to place in position u8Index of transmit buffer (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup buffer
*/
uint8_t ModbusMaster232::setTransmitBuffer(uint8_t u8Index, uint16_t u16Value){
  if (u8Index < ku8MaxBufferSize)
  {
    _u16TransmitBuffer[u8Index] = u16Value;
    return ku8MBSuccess;
  }
  else
  {
    return ku8MBIllegalDataAddress;
  }
}


/**
Clear Modbus transmit buffer.

@see ModbusMaster::setTransmitBuffer(uint8_t u8Index, uint16_t u16Value)
@ingroup buffer
*/
void ModbusMaster232::clearTransmitBuffer(){
  uint8_t i;
  
  for (i = 0; i < ku8MaxBufferSize; i++)
  {
    _u16TransmitBuffer[i] = 0;
  }
}


/**
Modbus function 0x01 Read Coils.

This function code is used to read from 1 to 2000 contiguous status of 
coils in a remote device. The request specifies the starting address, 
i.e. the address of the first coil specified, and the number of coils. 
Coils are addressed starting at zero.

The coils in the response buffer are packed as one coil per bit of the 
data field. Status is indicated as 1=ON and 0=OFF. The LSB of the first 
data word contains the output addressed in the query. The other coils 
follow toward the high order end of this word and from low order to high 
order in subsequent words.

If the returned quantity is not a multiple of sixteen, the remaining 
bits in the final data word will be padded with zeros (toward the high 
order end of the word).

@param u16ReadAddress address of first coil (0x0000..0xFFFF)
@param u16BitQty quantity of coils to read (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusMaster232::readCoils(uint16_t u16ReadAddress, uint16_t u16BitQty){
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16BitQty;
  return ModbusMasterTransaction(ku8MBReadCoils);
}


/**
Modbus function 0x02 Read Discrete Inputs.

This function code is used to read from 1 to 2000 contiguous status of 
discrete inputs in a remote device. The request specifies the starting 
address, i.e. the address of the first input specified, and the number 
of inputs. Discrete inputs are addressed starting at zero.

The discrete inputs in the response buffer are packed as one input per 
bit of the data field. Status is indicated as 1=ON; 0=OFF. The LSB of 
the first data word contains the input addressed in the query. The other 
inputs follow toward the high order end of this word, and from low order 
to high order in subsequent words.

If the returned quantity is not a multiple of sixteen, the remaining 
bits in the final data word will be padded with zeros (toward the high 
order end of the word).

@param u16ReadAddress address of first discrete input (0x0000..0xFFFF)
@param u16BitQty quantity of discrete inputs to read (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusMaster232::readDiscreteInputs(uint16_t u16ReadAddress,uint16_t u16BitQty){
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16BitQty;
  return ModbusMasterTransaction(ku8MBReadDiscreteInputs);
}


/**
Modbus function 0x03 Read Holding Registers.

This function code is used to read the contents of a contiguous block of 
holding registers in a remote device. The request specifies the starting 
register address and the number of registers. Registers are addressed 
starting at zero.

The register data in the response buffer is packed as one word per 
register.

@param u16ReadAddress address of the first holding register (0x0000..0xFFFF)
@param u16ReadQty quantity of holding registers to read (1..125, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster232::readHoldingRegisters(uint16_t u16ReadAddress,uint16_t u16ReadQty){
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  return ModbusMasterTransaction(ku8MBReadHoldingRegisters);
}


/**
Modbus function 0x04 Read Input Registers.

This function code is used to read from 1 to 125 contiguous input 
registers in a remote device. The request specifies the starting 
register address and the number of registers. Registers are addressed 
starting at zero.

The register data in the response buffer is packed as one word per 
register.

@param u16ReadAddress address of the first input register (0x0000..0xFFFF)
@param u16ReadQty quantity of input registers to read (1..125, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster232::readInputRegisters(uint16_t u16ReadAddress,uint8_t u16ReadQty){
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  return ModbusMasterTransaction(ku8MBReadInputRegisters);
}


/**
Modbus function 0x05 Write Single Coil.

This function code is used to write a single output to either ON or OFF 
in a remote device. The requested ON/OFF state is specified by a 
constant in the state field. A non-zero value requests the output to be 
ON and a value of 0 requests it to be OFF. The request specifies the 
address of the coil to be forced. Coils are addressed starting at zero.

@param u16WriteAddress address of the coil (0x0000..0xFFFF)
@param u8State 0=OFF, non-zero=ON (0x00..0xFF)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusMaster232::writeSingleCoil(uint16_t u16WriteAddress, uint8_t u8State){
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = (u8State ? 0xFF00 : 0x0000);
  return ModbusMasterTransaction(ku8MBWriteSingleCoil);
}


/**
Modbus function 0x06 Write Single Register.

This function code is used to write a single holding register in a 
remote device. The request specifies the address of the register to be 
written. Registers are addressed starting at zero.

@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16WriteValue value to be written to holding register (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster232::writeSingleRegister(uint16_t u16WriteAddress,uint16_t u16WriteValue){
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = 0;
  _u16TransmitBuffer[0] = u16WriteValue;
  return ModbusMasterTransaction(ku8MBWriteSingleRegister);
}


/**
Modbus function 0x0F Write Multiple Coils.

This function code is used to force each coil in a sequence of coils to 
either ON or OFF in a remote device. The request specifies the coil 
references to be forced. Coils are addressed starting at zero.

The requested ON/OFF states are specified by contents of the transmit 
buffer. A logical '1' in a bit position of the buffer requests the 
corresponding output to be ON. A logical '0' requests it to be OFF.

@param u16WriteAddress address of the first coil (0x0000..0xFFFF)
@param u16BitQty quantity of coils to write (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t ModbusMaster232::writeMultipleCoils(uint16_t u16WriteAddress,uint16_t u16BitQty){
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = u16BitQty;
  return ModbusMasterTransaction(ku8MBWriteMultipleCoils);
}

uint8_t ModbusMaster232::writeMultipleCoils(){
  _u16WriteQty = u16TransmitBufferLength;
  return ModbusMasterTransaction(ku8MBWriteMultipleCoils);
}


/**
Modbus function 0x10 Write Multiple Registers.

This function code is used to write a block of contiguous registers (1 
to 123 registers) in a remote device.

The requested written values are specified in the transmit buffer. Data 
is packed as one word per register.

@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16WriteQty quantity of holding registers to write (1..123, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster232::writeMultipleRegisters(uint16_t u16WriteAddress,uint16_t u16WriteQty){
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = u16WriteQty;
  return ModbusMasterTransaction(ku8MBWriteMultipleRegisters);
}

// new version based on Wire.h
uint8_t ModbusMaster232::writeMultipleRegisters(){
  _u16WriteQty = _u8TransmitBufferIndex;
  return ModbusMasterTransaction(ku8MBWriteMultipleRegisters);
}


/**
Modbus function 0x16 Mask Write Register.

This function code is used to modify the contents of a specified holding 
register using a combination of an AND mask, an OR mask, and the 
register's current contents. The function can be used to set or clear 
individual bits in the register.

The request specifies the holding register to be written, the data to be 
used as the AND mask, and the data to be used as the OR mask. Registers 
are addressed starting at zero.

The function's algorithm is:

Result = (Current Contents && And_Mask) || (Or_Mask && (~And_Mask))

@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16AndMask AND mask (0x0000..0xFFFF)
@param u16OrMask OR mask (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster232::maskWriteRegister(uint16_t u16WriteAddress,uint16_t u16AndMask, uint16_t u16OrMask){
  _u16WriteAddress = u16WriteAddress;
  _u16TransmitBuffer[0] = u16AndMask;
  _u16TransmitBuffer[1] = u16OrMask;
  return ModbusMasterTransaction(ku8MBMaskWriteRegister);
}


/**
Modbus function 0x17 Read Write Multiple Registers.

This function code performs a combination of one read operation and one 
write operation in a single MODBUS transaction. The write operation is 
performed before the read. Holding registers are addressed starting at 
zero.

The request specifies the starting address and number of holding 
registers to be read as well as the starting address, and the number of 
holding registers. The data to be written is specified in the transmit 
buffer.

@param u16ReadAddress address of the first holding register (0x0000..0xFFFF)
@param u16ReadQty quantity of holding registers to read (1..125, enforced by remote device)
@param u16WriteAddress address of the first holding register (0x0000..0xFFFF)
@param u16WriteQty quantity of holding registers to write (1..121, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t ModbusMaster232::readWriteMultipleRegisters(uint16_t u16ReadAddress,uint16_t u16ReadQty, uint16_t u16WriteAddress, uint16_t u16WriteQty){
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  _u16WriteAddress = u16WriteAddress;
  _u16WriteQty = u16WriteQty;
  return ModbusMasterTransaction(ku8MBReadWriteMultipleRegisters);
}

uint8_t ModbusMaster232::readWriteMultipleRegisters(uint16_t u16ReadAddress,uint16_t u16ReadQty){
  _u16ReadAddress = u16ReadAddress;
  _u16ReadQty = u16ReadQty;
  _u16WriteQty = _u8TransmitBufferIndex;
  return ModbusMasterTransaction(ku8MBReadWriteMultipleRegisters);
}


/* _____PRIVATE FUNCTIONS____________________________________________________ */
/**
Modbus transaction engine.
Sequence:
  - assemble Modbus Request Application Data Unit (ADU),
    based on particular function called
  - transmit request over selected serial port
  - wait for/retrieve response
  - evaluate/disassemble response
  - return status (success/exception)

@param u8MBFunction Modbus function (0x01..0xFF)
@return 0 on success; exception number on failure
*/
uint8_t ModbusMaster232::ModbusMasterTransaction(uint8_t u8MBFunction){
  uint8_t u8ModbusADU[256];
  uint8_t u8ModbusADUSize = 0;
  uint8_t i, u8Qty;
  uint16_t u16CRC;
  uint32_t u32StartTime;
  uint8_t u8BytesLeft = 8;
  uint8_t u8MBStatus = ku8MBSuccess;
  
  // assemble Modbus Request Application Data Unit
  u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;
  u8ModbusADU[u8ModbusADUSize++] = u8MBFunction;
  
  switch(u8MBFunction)
  {
    case ku8MBReadCoils:
    case ku8MBReadDiscreteInputs:
    case ku8MBReadInputRegisters:
    case ku8MBReadHoldingRegisters:
    case ku8MBReadWriteMultipleRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadAddress);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadAddress);
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty);
      break;
  }
  
  switch(u8MBFunction)
  {
    case ku8MBWriteSingleCoil:
    case ku8MBMaskWriteRegister:
    case ku8MBWriteMultipleCoils:
    case ku8MBWriteSingleRegister:
    case ku8MBWriteMultipleRegisters:
    case ku8MBReadWriteMultipleRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteAddress);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteAddress);
      break;
  }
  
  switch(u8MBFunction)
  {
    case ku8MBWriteSingleCoil:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
      break;
      
    case ku8MBWriteSingleRegister:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[0]);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[0]);
      break;
      
    case ku8MBWriteMultipleCoils:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
      u8Qty = (_u16WriteQty % 8) ? ((_u16WriteQty >> 3) + 1) : (_u16WriteQty >> 3);
      u8ModbusADU[u8ModbusADUSize++] = u8Qty;
      for (i = 0; i < u8Qty; i++)
      {
        switch(i % 2)
        {
          case 0: // i is even
            u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[i >> 1]);
            break;
            
          case 1: // i is odd
            u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[i >> 1]);
            break;
        }
      }
      break;
      
    case ku8MBWriteMultipleRegisters:
    case ku8MBReadWriteMultipleRegisters:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty << 1);
      
      for (i = 0; i < lowByte(_u16WriteQty); i++)
      {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[i]);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[i]);
      }
      break;
      
    case ku8MBMaskWriteRegister:
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[0]);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[0]);
      u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[1]);
      u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[1]);
      break;
  }
  
  
  u16CRC = 0xFFFF;
  for (i = 0; i < u8ModbusADUSize; i++)
  {
  //Function  crc16  for ESP8266   - PDAControl
    u16CRC = _crc16_update2(u16CRC, u8ModbusADU[i]);
  }
  u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
  u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);
  u8ModbusADU[u8ModbusADUSize] = 0;
  
  // transmit request
  for (i = 0; i < u8ModbusADUSize; i++)
  {
    port->print(char(u8ModbusADU[i]));
    //.print(char(u8ModbusADU[i]));
  }

  delay(2);
  
  u8ModbusADUSize = 1;
  
  // loop until we run out of time or bytes, or an error occurs
  u32StartTime = millis();

    int val = 0xFF;
    long cont = 0;
    
    while((val != _u8MBSlave) && (cont < timeout)) {       //original use 100 <--> MB_TIMEOUT
      val = port->read();
      delay(5);
      cont ++;
    }
    
    u8ModbusADU[u8ModbusADUSize] = val;
  
  
  while (u8BytesLeft && !u8MBStatus)
  {
  //if (Serial.available())
  if  (port->available())
  {    
    u8ModbusADU[u8ModbusADUSize] = port->read();
    u8BytesLeft--;
    u8ModbusADUSize ++;
     
  }
    else
    {

      if (_idle)
      {
        _idle();
      }

    }
    
    // evaluate slave ID, function code once enough bytes have been read
    if (u8ModbusADUSize == 5)
    {
  
      // verify response is for correct Modbus slave
      if (u8ModbusADU[0] != _u8MBSlave)
      {
        u8MBStatus = ku8MBInvalidSlaveID;
        break;
      }
      
      // verify response is for correct Modbus function code (mask exception bit 7)
      if ((u8ModbusADU[1] & 0x7F) != u8MBFunction)
      {
        u8MBStatus = ku8MBInvalidFunction;
        break;
      }
      
      // check whether Modbus exception occurred; return Modbus Exception Code
      if (bitRead(u8ModbusADU[1], 7))
      {
        u8MBStatus = u8ModbusADU[2];
        break;
      }
      
      // evaluate returned Modbus function code
      switch(u8ModbusADU[1])
      {
        case ku8MBReadCoils:
        case ku8MBReadDiscreteInputs:
        case ku8MBReadInputRegisters:
        case ku8MBReadHoldingRegisters:
        case ku8MBReadWriteMultipleRegisters:
          u8BytesLeft = u8ModbusADU[2];
          break;
          
        case ku8MBWriteSingleCoil:
        case ku8MBWriteMultipleCoils:
        case ku8MBWriteSingleRegister:
        case ku8MBWriteMultipleRegisters:
          u8BytesLeft = 3;
          break;
          
        case ku8MBMaskWriteRegister:
          u8BytesLeft = 5;
          break;
      }
    }
    if (millis() > (u32StartTime + ku8MBResponseTimeout))
    {
      u8MBStatus = ku8MBResponseTimedOut;
    }
  }
  
  // verify response is large enough to inspect further
  if (!u8MBStatus && u8ModbusADUSize >= 5)
  {
    // calculate CRC
    u16CRC = 0xFFFF;
    for (i = 0; i < (u8ModbusADUSize - 2); i++)
    {
    //Function  crc16  for ESP8266   - PDAControl
      u16CRC = _crc16_update2(u16CRC, u8ModbusADU[i]);
    }
    
  
  
  
    // verify CRC
    if (!u8MBStatus && (lowByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 2] ||
      highByte(u16CRC) != u8ModbusADU[u8ModbusADUSize - 1]))
    {
      u8MBStatus = ku8MBInvalidCRC;
    }
  }

  // disassemble ADU into words
  if (!u8MBStatus)
  {
    // evaluate returned Modbus function code
    switch(u8ModbusADU[1])
    {
      case ku8MBReadCoils:
      case ku8MBReadDiscreteInputs:
        // load bytes into word; response bytes are ordered L, H, L, H, ...
        for (i = 0; i < (u8ModbusADU[2] >> 1); i++)
        {
          if (i < ku8MaxBufferSize)
          {
      
        //Function makeWord Modified to ESP8266 - PDAControl
      //modificado for ESP8266
           // _u16ResponseBuffer[i] = makeWord(u8ModbusADU[2 * i + 4], u8ModbusADU[2 * i + 3]);
      
       _u16ResponseBuffer[i] = (u8ModbusADU[2 * i + 4] << 8) | u8ModbusADU[2 * i + 3];
      
          }
          
          _u8ResponseBufferLength = i;
        }
        
        // in the event of an odd number of bytes, load last byte into zero-padded word
        if (u8ModbusADU[2] % 2)
        {
          if (i < ku8MaxBufferSize)
          {
          //Function makeWord Modified to ESP8266 - PDAControl
        
           // _u16ResponseBuffer[i] = makeWord(0, u8ModbusADU[2 * i + 3]);
      
        _u16ResponseBuffer[i] =  (0 << 8) | u8ModbusADU[2 * i + 3];
        //return (h << 8) | l;
          }
          
          _u8ResponseBufferLength = i + 1;
        }
        break;
        
      case ku8MBReadInputRegisters:
      case ku8MBReadHoldingRegisters:
      case ku8MBReadWriteMultipleRegisters:
        // load bytes into word; response bytes are ordered H, L, H, L, ...
        for (i = 0; i < (u8ModbusADU[2] >> 1); i++)
        {
          if (i < ku8MaxBufferSize)
          {
          //Function makeWord Modified to ESP8266 - PDAControl
        
           // _u16ResponseBuffer[i] = makeWord(u8ModbusADU[2 * i + 3], u8ModbusADU[2 * i + 4]);
      
          //return (h << 8) | l;
          _u16ResponseBuffer[i] = (u8ModbusADU[2 * i + 3] << 8) | u8ModbusADU[2 * i + 4];
          }
          
          _u8ResponseBufferLength = i;
        }
        break;
    }
  }

  
  _u8TransmitBufferIndex = 0;
  u16TransmitBufferLength = 0;
  _u8ResponseBufferIndex = 0;
  return u8MBStatus;
}

/*

MakeWord function deleted -for ESP8266 PDAControl


unsigned int ModbusMaster232::makeWord(unsigned int w)
{
  return w;
}


unsigned int ModbusMaster232::makeWord(uint8_t h, uint8_t l)
{
  return (h << 8) | l;
}
*/
void ModbusMaster232::setSlaveAddress(uint8_t u8MBSlave){

  _u8MBSlave = u8MBSlave;
}

/*
Edited by Suratin
*/
uint8_t ModbusMaster232::readCoilsB(uint8_t _slave,uint16_t _address){
  uint16_t Temp;
  uint8_t Result;
  
  setSlaveAddress(_slave);
  
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readCoils(_address,1);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));

  delay(5);
  
  if(readSts == ku8MBSuccess){
    Temp=getResponseBuffer(0);
    if(Temp&=0x01){
      Result = 1;
    }else{
      Result = 0;
    }
  }else if(readSts != ku8MBSuccess){
    Result = 0xFF;
  }

  clearResponseBuffer();
  callback_func(0x01,_address,Result,0,0);
  return Result;
}
uint16_t ModbusMaster232::readCoilsU(uint8_t _slave,uint16_t _address,uint16_t _readcount){
  uint16_t TempU16;
  
  setSlaveAddress(_slave);
  
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readCoils(_address,_readcount);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  delay(5);
  
  if(readSts == ku8MBSuccess){
    TempU16 = getResponseBuffer(0);
  }else if(readSts != ku8MBSuccess){
    TempU16 = 0xFFFF;
  }
  
  clearResponseBuffer();
  callback_func(0x01,_address,0,TempU16,0);
  return TempU16;
}
uint8_t ModbusMaster232::readDiscreteInputsB(uint8_t _slave,uint16_t _address,uint8_t _prefferadd){
  uint16_t Temp;
  uint8_t Result;
  
  setSlaveAddress(_slave);
  
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readDiscreteInputs(_address,8);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  //readDiscreteInputs(_address,readcount);
  delay(5);
  
  if(readSts == ku8MBSuccess){
    Temp=getResponseBuffer(0);
    switch(_prefferadd) {
    case 0:
     if(Temp&=0x01){
      Result = true;
     }else{
      Result = false;
     }
     break;
    case 1:
     if(Temp&=0x02){
      Result = true;
     }else{
      Result = false;
     }
     break; 
    case 2:
     if(Temp&=0x04){
      Result = true;
     }else{
      Result = false;
     }
     break;  
    case 3:
     if(Temp&=0x08){
      Result = true;
     }else{
      Result = false;
     }
     break;  
    }
  }else if(readSts != ku8MBSuccess){
    Result = 0xFF;
  }
  
  //Temp=getResponseBuffer(0);


  clearResponseBuffer();
  //clearTransmitBuffer()
  callback_func(0x02,_address,Result,0,0);
  return Result;
}
uint16_t ModbusMaster232::readDiscreteInputsU(uint8_t _slave,uint16_t _address,uint16_t _readcount){
  uint16_t TempU16;
  
  setSlaveAddress(_slave);
  
  readDiscreteInputs(_address,_readcount);
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readDiscreteInputs(_address,_readcount);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  delay(5);
  
  if(readSts == ku8MBSuccess){
    TempU16 = getResponseBuffer(0);
  }else if(readSts != ku8MBSuccess){
    TempU16 = 0xFFFF;
  }
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  callback_func(0x02,_address,0,TempU16,0);
  return TempU16;
}
uint8_t ModbusMaster232::writeSingleCoilB(uint8_t _slave,uint16_t _address, uint8_t u8State){
 uint8_t res;

 clearResponseBuffer();
 setSlaveAddress(_slave);
 res = writeSingleCoil(_address,u8State);
 delay(5);
 clearResponseBuffer();
 return res;
}
uint16_t ModbusMaster232::readHoldingRegistersI(uint8_t _slave,uint16_t _address){
  uint16_t Temp;
  
  setSlaveAddress(_slave);
  
  //readHoldingRegisters(_address,1);
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readHoldingRegisters(_address,1);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  delay(5);
  
  //Temp=getResponseBuffer(0);
  if(readSts == ku8MBSuccess){
    Temp = getResponseBuffer(0);
  }else if(readSts != ku8MBSuccess){
    Temp = 0xFFFF;
  }
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  callback_func(0x03,_address,0,Temp,0);
  return Temp;
}
uint16_t ModbusMaster232::readInputRegistersI(uint8_t _slave,uint16_t _address){
  uint16_t Temp;
  
  setSlaveAddress(_slave);
  
  //readHoldingRegisters(_address,1);
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readInputRegisters(_address,1);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  delay(5);
  
  //Temp=getResponseBuffer(0);
  if(readSts == ku8MBSuccess){
    Temp = getResponseBuffer(0);
  }else if(readSts != ku8MBSuccess){
    Temp = 0xFFFF;
  }
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  callback_func(0x04,_address,0,Temp,0);
  return Temp;
}
void ModbusMaster232::readHoldingRegistersIn(uint8_t _slave,uint16_t _address,uint16_t _readCount,uint16_t* pArr){
  //uint16_t Temp[_readCount];
  //uint16_t *pTemp;
  
  setSlaveAddress(_slave); 

  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readHoldingRegisters(_address,_readCount);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));

  //pTemp = Temp;
  
  delay(5);
  
  if(readSts == ku8MBSuccess){
    for(uint8_t j=0;j<_readCount;j++){
      //Temp[j] = getResponseBuffer(j);
      *(pArr+j) = getResponseBuffer(j);
    }    
  }else if(readSts != ku8MBSuccess){
    for(uint8_t i=0;i<_readCount;i++){
      //Temp[i] = 0xFFFF;
      *(pArr+i) = 0xFFFF;
    }
  }
  
  //pArr = Temp;
  
  clearResponseBuffer();
  
}
float ModbusMaster232::readHoldingRegistersF(uint8_t _slave,uint16_t _address){
  uint16_t Temp[2];
  float fNum;
  
  setSlaveAddress(_slave);
  
  //readHoldingRegisters(_address,2);
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readHoldingRegisters(_address,2);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  delay(5);
  
  //Temp[0]=getResponseBuffer(0);
  //Temp[1]=getResponseBuffer(1);
  if(readSts == ku8MBSuccess){
    Temp[0]=getResponseBuffer(0);
    Temp[1]=getResponseBuffer(1);
  }else if(readSts != ku8MBSuccess){
    Temp[0] = 0xFFFF;
    Temp[1] = 0xFFFF;
  }
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  fNum = * (float*) &Temp[0];
  callback_func(0x03,_address,0,0,fNum);
  return fNum;
}
float ModbusMaster232::readHoldingRegistersFI(uint8_t _slave,uint16_t _address){
  uint16_t Temp[2];
  float fNum;
  
  setSlaveAddress(_slave);
  
  //readHoldingRegisters(_address,2);
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readHoldingRegisters(_address,2);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  delay(5);
  
  //Temp[1]=getResponseBuffer(0);
  //Temp[0]=getResponseBuffer(1);
  if(readSts == ku8MBSuccess){
    Temp[1]=getResponseBuffer(0);
    Temp[0]=getResponseBuffer(1);
  }else if(readSts != ku8MBSuccess){
    Temp[0] = 0xFFFF;
    Temp[1] = 0xFFFF;
  }
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  fNum = * (float*) &Temp[0];
  callback_func(0x03,_address,0,0,fNum);
  //Pulse_num = *(int32_t*) &iSlave.Hreg(100);
  return fNum;
}
float ModbusMaster232::readHoldingRegistersFAI(uint8_t _slave,uint16_t _address){
  uint16_t Temp[2];
  uint8_t uTemp[4];
  float fNum;
  
  setSlaveAddress(_slave);
  
  //readHoldingRegisters(_address,2);
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readHoldingRegisters(_address,2);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  delay(5);
  
  if(readSts == ku8MBSuccess){
    Temp[0]=getResponseBuffer(0);
    Temp[1]=getResponseBuffer(1);
   
    uTemp[1] = (uint8_t)Temp[0];  //Swap 4 byte        
    uTemp[0] = (uint8_t)(Temp[0] >> 8);
    uTemp[3] = (uint8_t)Temp[1];
    uTemp[2] = (uint8_t)(Temp[1] >> 8);
    
    Temp[0] = * (uint16_t*) &uTemp[0];
    Temp[1] = * (uint16_t*) &uTemp[2];
  }else if(readSts != ku8MBSuccess){
    Temp[0] = 0xFFFF;
    Temp[1] = 0xFFFF;
  }
  
  clearResponseBuffer();
  
  fNum = * (float*) &Temp[0];
  callback_func(0x03,_address,0,0,fNum);
  return fNum;
}
float ModbusMaster232::readInputRegistersF(uint8_t _slave,uint16_t _address){
  uint16_t Temp[2];
  float fNum;
  
  setSlaveAddress(_slave);
  
  //readInputRegisters(_address,2);
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readInputRegisters(_address,2);
    if(readSts != ku8MBSuccess){
      delay(10);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  delay(5);
  
  if(readSts == ku8MBSuccess){
    Temp[0]=getResponseBuffer(0);
    Temp[1]=getResponseBuffer(1);

  }else if(readSts != ku8MBSuccess){
    Temp[0] = 0xFFFF;
    Temp[1] = 0xFFFF;
  }
  
  //Temp[0]=getResponseBuffer(0);
  //Temp[1]=getResponseBuffer(1);
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  fNum = * (float*) &Temp[0];
  callback_func(0x04,_address,0,0,fNum);
  return fNum;
}
float ModbusMaster232::readInputRegistersFI(uint8_t _slave,uint16_t _address){
  uint16_t Temp[2];
  float fNum;
  
  setSlaveAddress(_slave);
  
  //readInputRegisters(_address,2);
  /*---------------------------------------------
               Read from modbus routine
  ----------------------------------------------*/
  uint8_t readSts;
  uint8_t retryCount = 0;
  do{
    readSts = readInputRegisters(_address,2);
    if(readSts != ku8MBSuccess){
      delay(100);    //if error status ,wait 10 ms before next request
      retryCount++;
    }
  }while((readSts != ku8MBSuccess)&&(retryCount < tryCount));
  
  delay(5);
  
  if(readSts == ku8MBSuccess){
    Temp[1]=getResponseBuffer(0);
    Temp[0]=getResponseBuffer(1);
  }else if(readSts != ku8MBSuccess){
    Temp[0] = 0xFFFF;
    Temp[1] = 0xFFFF;
  }

  //Serial.println(retryCount);
  //Serial.println(Temp[1]);
  //Serial.println(Temp[0]);
  
  //Temp[1]=getResponseBuffer(0);
  //Temp[0]=getResponseBuffer(1);
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  fNum = * (float*) &Temp[0];
  callback_func(0x04,_address,0,0,fNum);
  return fNum;
}

bool ModbusMaster232::writeSingleRegisterF(uint8_t _slave,uint16_t _address,float _data){
  uint16_t Temp[sizeof(float)];
  
  setSlaveAddress(_slave);
  
  *(float*)(Temp) = _data;
  
  writeSingleRegister(_address,Temp[0]);
  delay(5);
  writeSingleRegister(_address+1,Temp[1]);
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  
  return true;
}
bool ModbusMaster232::writeSingleRegisterFI(uint8_t _slave,uint16_t _address,float _data){
  uint16_t Temp[sizeof(float)];
  
  setSlaveAddress(_slave);
  
  *(float*)(Temp) = _data;
  
  writeSingleRegister(_address,Temp[1]);
  delay(5);
  writeSingleRegister(_address+1,Temp[0]);
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  
  return true;
}
bool ModbusMaster232::writeSingleRegisterFAI(uint8_t _slave,uint16_t _address,float _data){
  uint16_t Temp[2];
  uint8_t Temp2[4];
  uint8_t _result;
  
  setSlaveAddress(_slave);
  
  *(float*)(Temp) = _data;
  
  Temp2[1] = (uint8_t)Temp[0];
  Temp2[0] = (uint8_t)(Temp[0] >> 8);
  Temp2[3] = (uint8_t)Temp[1];
  Temp2[2] = (uint8_t)(Temp[1] >> 8);
  
  Temp[0] = *(uint16_t*)&Temp2[0];
  Temp[1] = *(uint16_t*)&Temp2[2];
  
  setTransmitBuffer(0,Temp[0]);
  setTransmitBuffer(1,Temp[1]);
  
  _result = writeMultipleRegisters(_address,2);
  
  //writeSingleRegister(_address,Temp[0]);
  //delay(5);
  //writeSingleRegister(_address+1,Temp[1]);
  
  clearResponseBuffer();
  //clearTransmitBuffer();
  
  return _result;
}
bool ModbusMaster232::writeSingleRegisterI(uint8_t _slave,uint16_t _address,int16_t _data){
  //uint16_t Temp[sizeof(float)];
  uint8_t result;
  
  setSlaveAddress(_slave);
  
  //*(float*)(Temp) = _data;
  
   result = writeSingleRegister(_address,_data);
  //writeSingleRegister(_address+1,Temp[1]);
  
  clearResponseBuffer();
  return result;
}

void ModbusMaster232::setCallback(void(*_callBack)(uint8_t func, uint16_t sID ,bool iBit ,uint16_t iVal ,float fVal)){
  callback_func = _callBack;
}