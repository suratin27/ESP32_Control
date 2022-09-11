#include "ModbusTTCP.h"

bool debug = false;
int toooo;

unsigned long lastMills = 0;
uint8_t waitCount = 0;
const char* ErrorName[] = {
  "IllegalFunction",
  "IllegalDataAddress",
  "IllegalDataValue",
  "ServerDeviceFailure"
};
const uint8_t hexmask[] = {
  0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80
};

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusFrame::ModbusFrame(uint8_t slave, uint8_t *pdu) {
	_slave = slave;
	_pdu = pdu;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusResponse::ModbusResponse(uint8_t slave, uint8_t *pdu) : ModbusFrame(slave, pdu) {

}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusResponse::isDiscreteSet(uint16_t offset) const {
	if (_pdu == nullptr) {
		return false;
	}

	// ptr = PDU + FC (1) + BYTE COUNT (1) + OFFSET (offset / 8)
	uint8_t *ptr = _pdu + 2 + (offset >> 3);
	offset &= 0x07; // offset is the bit number
	return (*ptr >> offset) & 0x01;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t ModbusResponse::getRegister(uint16_t offset) const {
	if (_pdu == nullptr) {
		return 0;
	}

	// ptr = PDU + FC(1) + BYTE COUNT (1) + OFFSET (offset * 2)
	uint8_t *ptr = _pdu + 2 + (offset << 1);
	return (uint16_t(*ptr) << 8) + *(ptr + 1); // MSB + LSB
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusMaster::ModbusMaster() {
	setState(Idle);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusMaster::sendReadRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t quantity) {
	switch (fc) {
		case ReadCoils:
		case ReadDiscreteInputs:
			if (quantity < 1 || quantity > 2000) {
				// Invalid quantity
				return false;
			}
			break;

		case ReadHoldingRegisters:
		case ReadInputRegisters:
			if (quantity < 1 || quantity > 125) {
				// Invalid quantity
				return false;
			}
			break;

		default:
			// Invalid FC
			return false;
	}

	if (!prepareRequest(slave, fc)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_next++ = addr >> 8;
	*_next++ = addr;
	*_next++ = quantity >> 8;
	*_next++ = quantity;

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusMaster::sendWriteSingleRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t value) {
	switch (fc) {
		case WriteSingleCoil:
		case WriteSingleRegister:
			break;

		default:
			// Invalid FC
			return false;
	}

	if (!prepareRequest(slave, fc)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_next++ = addr >> 8;
	*_next++ = addr;
	*_next++ = value >> 8;
	*_next++ = value;

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusMaster::sendWriteMultipleCoilsRequest(uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity) {
	if (quantity < 1 || quantity > 0x07b0) {
		// Invalid quantity
		return false;
	}

	if (!prepareRequest(slave, WriteMultipleCoils)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_next++ = addr >> 8;
	*_next++ = addr;
	*_next++ = quantity >> 8;
	*_next++ = quantity;
	*_next++ = ((quantity - 1) >> 3) + 1;

	uint8_t bit = 0;
	const bool *ptr = values;
	*_next = 0;
	while (quantity > 0) {
		*_next |= (*ptr++ ? 1 : 0) << bit;
		if (bit == 7) {
			bit = 0;
			*(++_next) = 0;
		} else {
			++bit;
		}
		--quantity;
	}
	if (bit > 0) {
		++_next;
	}

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusMaster::sendWriteMultipleRegistersRequest(uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity) {
	if (quantity < 1 || quantity > 0x007b) {
		// Invalid quantity
		return false;
	}

	if (!prepareRequest(slave, WriteMultipleRegisters)) {
		// Request prepare error
		return false;
	}

	// DATA
	*_next++ = addr >> 8;
	*_next++ = addr;
	*_next++ = quantity >> 8;
	*_next++ = quantity;
	*_next++ = quantity << 1;

	const uint16_t *ptr = values;
	while (quantity-- > 0) {
		*_next++ = *ptr >> 8;
		*_next++ = *ptr++;
	}

	// START SEND PROCESS
	return sendRequest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusTCPMaster::ModbusTCPMaster() {
	_currentTransactionID = 0;
	_currentClient = nullptr;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::_readCoils(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity) {
	_currentClient = &client;
	return sendReadRequest(slave, ReadCoils, addr, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::_readDiscreteInputs(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity) {
	_currentClient = &client;
	return sendReadRequest(slave, ReadDiscreteInputs, addr, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::_readHoldingRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity) {
	_currentClient = &client;
	return sendReadRequest(slave, ReadHoldingRegisters, addr, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::_readInputRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity) {
	_currentClient = &client;
	return sendReadRequest(slave, ReadInputRegisters, addr, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::writeSingleCoil(WiFiClient &client, uint8_t slave, uint16_t addr, bool value) {
	_currentClient = &client;
	return sendWriteSingleRequest(slave, WriteSingleCoil, addr, value ? 0xFF00 : 0x0000);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::writeSingleRegister(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t value) {
	_currentClient = &client;
	return sendWriteSingleRequest(slave, WriteSingleRegister, addr, value);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::writeMultipleCoils(WiFiClient &client, uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity) {
	_currentClient = &client;
	return sendWriteMultipleCoilsRequest(slave, addr, values, quantity);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::writeMultipleRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity) {
	_currentClient = &client;
	return sendWriteMultipleRegistersRequest(slave, addr, values, quantity);
}

void ModbusTCPMaster::readCoils(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, bool* storage){
  if(_readCoils(client,slave, addr, quantity)){
    #ifdef ESP32_CONTROL_DEBUG
      //Serial.println("Got data");
    #endif
  }else{
    #ifdef ESP32_CONTROL_DEBUG
      //Serial.println("Can not read data");
    #endif
  }
  if((millis() - lastMills > 20)&&(waitCount < 3)){
    lastMills = millis();
    // Check available responses often
    if (isWaitingResponse()) {
      ModbusResponse response = available();
      if (response) {
        //if (response.hasError()) {
        if(response.hasError()){
          #ifdef ESP32_CONTROL_DEBUG
            Serial.print("Error : ");
            Serial.println(ErrorName[response.getErrorCode()]);
          #endif
          //return false;
        } else {
          #ifdef ESP32_CONTROL_DEBUG
            Serial.print("Coil registers values: ");
          #endif
          int i=0;
          if(quantity <= 8){
            uint8_t iRes[2];
            *(uint16_t*)(iRes) = response.getRegister(0);
            //Serial.print("iRes0: ");
            //Serial.println(iRes[0]);
            //Serial.print("iRes1: ");
            //Serial.println(iRes[1]);
            for(i=0;i<quantity;i++){
              *(storage + i) = iRes[1]&hexmask[i];
              #ifdef ESP32_CONTROL_DEBUG
                Serial.print(*(storage + i));
                Serial.print(" , ");
              #endif
            }
            //Serial.println();
          }else{
            uint8_t tByte = 0;         //- Data store registor
            if((quantity%8)==0){
              tByte = quantity / 8;
            }else{
              tByte = (quantity / 8) + 1;
            }
            if((tByte%2)!=0){
              tByte += 1;        //- if tByte = 5 must +1
            }
            uint8_t iRes[tByte];
            uint8_t tWord = tByte/2;
            uint8_t byteTemp = 0;
            for(uint8_t x = 0;x<tWord;x++){
              *(((uint16_t*)(iRes))+x) = response.getRegister(x);
            }
            //Serial.println();
            for(uint8_t o=0;o<tByte;o++){
              byteTemp  = iRes[o*2];
              iRes[o*2]   = iRes[(o*2)+1];
              iRes[(o*2)+1] = byteTemp;
            }
            for(uint8_t op=0;op<tByte;op++){
              //Serial.print("iRes ");
              //Serial.print(op); 
              //Serial.print(" : ");
              //Serial.println(iRes[op]); 
            }
            uint8_t bitPos = 0;
            uint8_t iResPos = 0;
            for(uint8_t j=0;j<quantity;j++){
              if((iRes[iResPos]&hexmask[bitPos])!=0){
                *(storage + j) = true;
              }else{
                *(storage + j) = false;
              }
              
              #ifdef ESP32_CONTROL_DEBUG /*
                Serial.print("J loop: ");
                Serial.println(j);
                Serial.print("bitPos Count: ");
                Serial.println(bitPos);
                Serial.print("Hexmask: ");
                Serial.println(hexmask[bitPos]);
                Serial.print("bitPos Value: ");
                Serial.println(*(storage + j)); */
                Serial.print(*(storage + j));
                Serial.print(" , ");
              #endif
              if(bitPos <= 6){
                bitPos += 1;
              }else{
                iResPos += 1;
                bitPos = 0;
              }
            }
            //Serial.println();   
          }
        }
      }
    }
    waitCount += 1;
  }
  waitCount = 0;
  //delay(5);
}
void ModbusTCPMaster::readDiscreteInputs(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, bool* storage){
  if(_readDiscreteInputs(client,slave, addr, quantity)){
    #ifdef ESP32_CONTROL_DEBUG
      //Serial.println("Got data");
    #endif
  }else{
    #ifdef ESP32_CONTROL_DEBUG
      //Serial.println("Can not read data");
    #endif
  }
  // Check available responses often
  if (isWaitingResponse()) {
    ModbusResponse response = available();
    if (response) {
      //if (response.hasError()) {
      if(response.hasError()){
        #ifdef ESP32_CONTROL_DEBUG
          Serial.print("Error : ");
          Serial.println(ErrorName[response.getErrorCode()]);
        #endif
        //return false;
      } else {
        #ifdef ESP32_CONTROL_DEBUG
          Serial.print("DiscreteInput registers values: ");
        #endif
        int i=0;
        if(quantity <= 8){
          uint8_t iRes[2];
          *(uint16_t*)(iRes) = response.getRegister(0);
          for(i=0;i<quantity;i++){
            *(storage + i) = iRes[1]&hexmask[i];
            #ifdef ESP32_CONTROL_DEBUG
              Serial.print(*(storage + i));
              Serial.print(" , ");
            #endif
          }
          //Serial.println();
        }else{
          uint8_t tByte = 0;         //- Data store registor
          if((quantity%8)==0){
            tByte = quantity / 8;
          }else{
            tByte = (quantity / 8) + 1;
          }
          if((tByte%2)!=0){
            tByte += 1;        //- if tByte = 5 must +1
          }
          uint8_t iRes[tByte];
          uint8_t tWord = tByte/2;
          uint8_t byteTemp = 0;
          for(uint8_t x = 0;x<tWord;x++){
            *(((uint16_t*)(iRes))+x) = response.getRegister(x);
          }
          //Serial.println();
          for(uint8_t o=0;o<tByte;o++){
            byteTemp  = iRes[o*2];
            iRes[o*2]   = iRes[(o*2)+1];
            iRes[(o*2)+1] = byteTemp;
          }
          for(uint8_t op=0;op<tByte;op++){
            //Serial.print("iRes ");
            //Serial.print(op); 
            //Serial.print(" : ");
            //Serial.println(iRes[op]); 
          }
          uint8_t bitPos = 0;
          uint8_t iResPos = 0;
          for(uint8_t j=0;j<quantity;j++){
            if((iRes[iResPos]&hexmask[bitPos])!=0){
              *(storage + j) = true;
            }else{
              *(storage + j) = false;
            }
            
            #ifdef ESP32_CONTROL_DEBUG
              Serial.print("J loop: ");
              Serial.println(j);
              Serial.print("bitPos Count: ");
              Serial.println(bitPos);
              Serial.print("Hexmask: ");
              Serial.println(hexmask[bitPos]);
              Serial.print("bitPos Value: ");
              Serial.println(*(storage + j));
              Serial.print(*(storage + j));
              Serial.print(" , ");
            #endif
            if(bitPos <= 6){
              bitPos += 1;
            }else{
              iResPos += 1;
              bitPos = 0;
            }
          }
          //Serial.println();   
        }
      }
    }
  }
  /*
  if((millis() - lastMills > 20)&&(waitCount < 3)){
    lastMills = millis();
    // Check available responses often
    if (isWaitingResponse()) {
      ModbusResponse response = available();
      if (response) {
        //if (response.hasError()) {
        if(response.hasError()){
          #ifdef ESP32_CONTROL_DEBUG
            Serial.print("Error : ");
            Serial.println(ErrorName[response.getErrorCode()]);
          #endif
          //return false;
        } else {
          #ifdef ESP32_CONTROL_DEBUG
            Serial.print("DiscreteInput registers values: ");
          #endif
          int i=0;
          if(quantity <= 8){
            uint8_t iRes[2];
            *(uint16_t*)(iRes) = response.getRegister(0);
            for(i=0;i<quantity;i++){
              *(storage + i) = iRes[1]&hexmask[i];
              #ifdef ESP32_CONTROL_DEBUG
                Serial.print(*(storage + i));
                Serial.print(" , ");
              #endif
            }
            //Serial.println();
          }else{
            uint8_t tByte = 0;         //- Data store registor
            if((quantity%8)==0){
              tByte = quantity / 8;
            }else{
              tByte = (quantity / 8) + 1;
            }
            if((tByte%2)!=0){
              tByte += 1;        //- if tByte = 5 must +1
            }
            uint8_t iRes[tByte];
            uint8_t tWord = tByte/2;
            uint8_t byteTemp = 0;
            for(uint8_t x = 0;x<tWord;x++){
              *(((uint16_t*)(iRes))+x) = response.getRegister(x);
            }
            //Serial.println();
            for(uint8_t o=0;o<tByte;o++){
              byteTemp  = iRes[o*2];
              iRes[o*2]   = iRes[(o*2)+1];
              iRes[(o*2)+1] = byteTemp;
            }
            for(uint8_t op=0;op<tByte;op++){
              //Serial.print("iRes ");
              //Serial.print(op); 
              //Serial.print(" : ");
              //Serial.println(iRes[op]); 
            }
            uint8_t bitPos = 0;
            uint8_t iResPos = 0;
            for(uint8_t j=0;j<quantity;j++){
              if((iRes[iResPos]&hexmask[bitPos])!=0){
                *(storage + j) = true;
              }else{
                *(storage + j) = false;
              }
              
              #ifdef ESP32_CONTROL_DEBUG
                Serial.print("J loop: ");
                Serial.println(j);
                Serial.print("bitPos Count: ");
                Serial.println(bitPos);
                Serial.print("Hexmask: ");
                Serial.println(hexmask[bitPos]);
                Serial.print("bitPos Value: ");
                Serial.println(*(storage + j));
                Serial.print(*(storage + j));
                Serial.print(" , ");
              #endif
              if(bitPos <= 6){
                bitPos += 1;
              }else{
                iResPos += 1;
                bitPos = 0;
              }
            }
            //Serial.println();   
          }
        }
      }
    }
    waitCount += 1;
  }
  waitCount = 0;
  */
  //delay(5);
}
void ModbusTCPMaster::readHoldingRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, uint16_t* storage){
  if(_readHoldingRegisters(client,slave, addr, quantity)){
    #ifdef ESP32_CONTROL_DEBUG
      //Serial.println("Got data");
    #endif
  }else{
    #ifdef ESP32_CONTROL_DEBUG
      //Serial.println("Can not read data");
    #endif
  }
  if((millis() - lastMills > 20)&&(waitCount < 3)){
    lastMills = millis();
    // Check available responses often
    if (isWaitingResponse()) {
      ModbusResponse response = available();
      if (response) {
        //if (response.hasError()) {
        if(response.hasError()){
          #ifdef ESP32_CONTROL_DEBUG
            Serial.print("Error : ");
            Serial.println(ErrorName[response.getErrorCode()]);
          #endif
          //return false;
        } else {
          #ifdef ESP32_CONTROL_DEBUG
            Serial.print("Holding registers values: ");
          #endif
          for (int i = 0; i < quantity; ++i){
            *(storage + i) = response.getRegister(i);
            #ifdef ESP32_CONTROL_DEBUG
              Serial.print(*(storage + i));
              Serial.print(',');
            #endif
          }
			    //Serial.println();
        }
      }
    }
    waitCount += 1;
  }
  waitCount = 0;
  //delay(5);
}
void ModbusTCPMaster::readInputRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, uint16_t* storage){
  if(_readInputRegisters(client,slave, addr, quantity)){
    #ifdef ESP32_CONTROL_DEBUG
      //Serial.println("Got data");
    #endif
  }else{
    #ifdef ESP32_CONTROL_DEBUG
      //Serial.println("Can not read data");
    #endif
  }
  // Check available responses often
  if (isWaitingResponse()) {
    ModbusResponse response = available();
    if (response) {
      //if (response.hasError()) {
      if(response.hasError()){
        #ifdef ESP32_CONTROL_DEBUG
          Serial.print("Error : ");
          Serial.println(ErrorName[response.getErrorCode()]);
        #endif
        //return false;
      } else {
        #ifdef ESP32_CONTROL_DEBUG
          Serial.print("Input registers values: ");
        #endif
        for (int i = 0; i < quantity; ++i){
          *(storage + i) = response.getRegister(i);
          #ifdef ESP32_CONTROL_DEBUG
            Serial.print(*(storage + i));
            Serial.print(',');
          #endif
        }
        #ifdef ESP32_CONTROL_DEBUG
          Serial.println();
        #endif
      }
    }
  }
  /*
  if((millis() - lastMills > 20)&&(waitCount < 3)){
    lastMills = millis();
    // Check available responses often
    if (isWaitingResponse()) {
      ModbusResponse response = available();
      if (response) {
        //if (response.hasError()) {
        if(response.hasError()){
          #ifdef ESP32_CONTROL_DEBUG
            Serial.print("Error : ");
            Serial.println(ErrorName[response.getErrorCode()]);
          #endif
          //return false;
        } else {
          #ifdef ESP32_CONTROL_DEBUG
            Serial.print("Input registers values: ");
          #endif
          for (int i = 0; i < quantity; ++i){
            *(storage + i) = response.getRegister(i);
            #ifdef ESP32_CONTROL_DEBUG
              Serial.print(*(storage + i));
              Serial.print(',');
            #endif
          }
			    //Serial.println();
        }
      }
    }
    waitCount += 1;
  }
  waitCount = 0;
  */
  //delay(5);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::prepareRequest(uint8_t slave, uint8_t fc) {
	if (getState() != Idle) {
		// Invalid state
		return false;
	}

	_next = _adu;
	++_currentTransactionID;
	_currentSlave = slave;
	_currentFC = fc;

	// Header
	*_next++ = _currentTransactionID >> 8; // Transaction ID Hi
	*_next++ = _currentTransactionID; // Transaction ID Lo
	*_next++ = 0x00; // Protocol ID Hi
	*_next++ = 0x00; // Protocol ID Lo
	*_next++ = 0x00; // Length Hi
	*_next++ = 0x00; // Length Lo
	*_next++ = slave; // Unit ID
	*_next++ = fc; // Function code

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ModbusTCPMaster::sendRequest() {
	if (_currentClient == nullptr) {
		// No current client
		return false;
	}

	if (!_currentClient->connected()) {
		// Client not connected
		return false;
	}

	// Set length
	uint16_t len = _next - _adu - 6;
	_adu[4] = len >> 8; // Length Hi
	_adu[5] = len; // Length Lo

	// Ignore pending data
	while (_currentClient->available()) {
		_currentClient->read();
	}

	// Send the packet
	bool ret = _currentClient->write(_adu, _next - _adu) == uint16_t(_next - _adu);
	if (ret) {
		_lastRequestTime = millis();
		setState(WaitingResponse);
	}

	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
ModbusResponse ModbusTCPMaster::available() {
	uint8_t *responsePDU = nullptr;

	if (_currentClient) {
		if (getState() == Receiving || getState() == WaitingResponse) {
			if (_currentClient->available()) {
				if (getState() == WaitingResponse) {
					setState(Receiving);
					_next = _adu;
				}

				uint16_t responseLen = _next - _adu;

				do {
					if (responseLen >= MODBUS_TCP_ADU_SIZE) {
						// Overflow error
						setState(Idle);
						// TODO notify to user
						break;
					}

					// Receive next byte
					*_next++ = _currentClient->read();
					++responseLen;

					if (responseLen >= MODBUS_TCP_HEADER_SIZE) {
						// Packet length is available
						uint16_t packetLen = (_adu[4] << 8) + _adu[5];
						if (packetLen == responseLen - 6) {
							// Packet complete

							// Check errors
							uint16_t transactionID = (_adu[0] << 8) + _adu[1];
							if (transactionID != _currentTransactionID) {
								// Invalid transaction ID
								// TODO notify to user
							} else if (_adu[2] != 0x00 && _adu[3] != 0x00) {
								// Invalid protocol ID
								// TODO notify to user
							} else if (_adu[6] != _currentSlave) {
								// Bad slave error
								// TODO notify to user
							} else if ((_adu[7] & 0x7f) != _currentFC) {
								// Bad function code
								// TODO notify to user
							} else {
								// TODO Check data length

								// Valid response
								responsePDU = _adu + 7;
							}

							setState(Idle);
							break;
						}
					}
				} while (_currentClient->available());
			} else if (millis() - _lastRequestTime > MODBUS_TCP_RESPONSE_TIMEOUT) {
				// Response timeout error
				setState(Idle);
				// TODO notify to user
			}
		}
	}

	if (responsePDU != nullptr) {
		_currentClient = nullptr;
	}

	return ModbusResponse(_currentSlave, responsePDU);
}