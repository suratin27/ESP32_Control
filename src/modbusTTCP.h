#include <Arduino.h>
#include <WiFi.h>

#define MODBUS_DATA_SIZE				(252) // bytes
#define MODBUS_PDU_SIZE 				(1 + MODBUS_DATA_SIZE) // PDU = FC (1) + DATA
#define MODBUS_TCP_HEADER_SIZE			(7) // TRANSACTION ID (2) + PROTOCOL ID (2) + LENGTH (2) + UNIT ID (1)
#define MODBUS_TCP_ADU_SIZE				(MODBUS_TCP_HEADER_SIZE + MODBUS_PDU_SIZE) // ADU = HEADER + PDU
#define MODBUS_TCP_RESPONSE_TIMEOUT		1000 //ms

//#define ESP32_CONTROL_DEBUG

#define arrCount(x) (sizeof(x)/sizeof((x)[0]))

class ModbusDevice {
	public:
		enum FunctionCodes {
			ReadCoils				= 1,
			ReadDiscreteInputs		= 2,
			ReadHoldingRegisters	= 3,
			ReadInputRegisters		= 4,
			WriteSingleCoil			= 5,
			WriteSingleRegister		= 6,
			WriteMultipleCoils		= 15,
			WriteMultipleRegisters	= 16,
		};

		enum Errors {
			IllegalFunction			  = 0x01,
			IllegalDataAddress		= 0x02,
			IllegalDataValue		  = 0x03,
			ServerDeviceFailure		= 0x04,
		};

	protected:
		inline uint8_t getState() const {
			return _state;
		}

		inline void setState(uint8_t state) {
			_state = state;
		}

	private:
		uint8_t _state;
};

class ModbusFrame {
	public:
		explicit ModbusFrame(uint8_t slave, uint8_t *pdu = nullptr);

	public:
		inline bool isNull() const {
			return _pdu == nullptr;
		}

	public:
		inline operator bool() const {
			return !isNull();
		}

	protected:
		uint8_t *_pdu;
		uint8_t _slave;
};

class ModbusResponse : public ModbusFrame {
	public:
		explicit ModbusResponse(uint8_t slave, uint8_t *pdu = nullptr);

	public:
		inline bool hasError() const {
			return *_pdu & 0x80;
		}

		inline uint8_t getErrorCode() const {
			return _pdu[1];
		}

		inline uint8_t getSlave() const {
			return _slave;
		}

		inline uint8_t getFC() const {
			return *_pdu & 0x7f;
		}

		inline bool isCoilSet(uint16_t offset) const {
			return isDiscreteSet(offset);
		}

		inline bool isDiscreteInputSet(uint16_t offset) const {
			return isDiscreteSet(offset);
		}

		bool isDiscreteSet(uint16_t offset) const;
		uint16_t getRegister(uint16_t offset) const;
};

class ModbusMaster : public ModbusDevice {
	protected:
		enum States {
			Idle,
			PreSending,
			Sending,
			PostSending,
			WaitingResponse,
			Receiving,
			WaitingTurnaround,
		};

	public:
		explicit ModbusMaster();

	public:
		inline bool isIdle() const {
			return getState() == Idle;
		}
		inline bool isWaitingResponse() const {
			return !isIdle();
		}

		virtual ModbusResponse available() = 0;

	protected:
		bool sendReadRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t quantity);
		bool sendWriteSingleRequest(uint8_t slave, uint8_t fc, uint16_t addr, uint16_t value);
		bool sendWriteMultipleCoilsRequest(uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity);
		bool sendWriteMultipleRegistersRequest(uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity);

		virtual bool prepareRequest(uint8_t slave, uint8_t fc) = 0;
		virtual bool sendRequest() = 0;

	protected:
		uint8_t *_next;
};

class ModbusTCPMaster : public ModbusMaster {
	public:
		explicit ModbusTCPMaster();

	public:
		bool _readCoils(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
		bool _readDiscreteInputs(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
		bool _readHoldingRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
		bool _readInputRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity);
		bool writeSingleCoil(WiFiClient &client, uint8_t slave, uint16_t addr, bool value);
		bool writeSingleRegister(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t value);
		bool writeMultipleCoils(WiFiClient &client, uint8_t slave, uint16_t addr, const bool *values, uint16_t quantity);
		bool writeMultipleRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, const uint16_t *values, uint16_t quantity);

		void readCoils(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, bool* storage);
		void readDiscreteInputs(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, bool* storage);
    void readHoldingRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, uint16_t* storage);
		void readInputRegisters(WiFiClient &client, uint8_t slave, uint16_t addr, uint16_t quantity, uint16_t* storage);

		virtual ModbusResponse available();

	protected:
		virtual bool prepareRequest(uint8_t slave, uint8_t fc);
		virtual bool sendRequest();

	private:
		// State properties
		uint32_t _lastRequestTime;
		uint16_t _currentTransactionID;
		uint8_t _currentSlave;
		uint8_t _currentFC;
		WiFiClient *_currentClient;

		uint8_t _adu[MODBUS_TCP_ADU_SIZE];
};