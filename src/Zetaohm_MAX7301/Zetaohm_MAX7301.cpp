#include "Zetaohm_MAX7301.h"
#include <SPI.h>//this chip needs SPI

Zetaohm_MAX7301::Zetaohm_MAX7301(){
}

void Zetaohm_MAX7301::begin(const uint8_t csPin, uint32_t spispeed, bool protocolInitOverride) {
  _cs = csPin;
  //	postSetup(csPin);
	MAX7301_SPI = SPISettings(32000000, MSBFIRST, SPI_MODE0);//mode3

	SPI.begin();
	//SPI.setBitOrder(MSBFIRST);
	//SPI.setDataMode(SPI_MODE0);

	pinMode(_cs, OUTPUT);
	digitalWrite(_cs, HIGH);
	delay(100);
	fellBuffer = 0x0;
	roseBuffer = 0x0;
	// disable shutdown so chip works!
  writeByte(0x04, 0x01, 0);
  writeByte(0x04, 0x01, 1);
  // testing for 2
  // end testing for 2
  Serial.print("0 0x04: 0x");
  Serial.println(readAddress(0x04, 0), HEX);
    Serial.print("0 0x04: 0x");
    Serial.println(readAddress(0x04, 0), HEX);
    Serial.print("1 0x04: 0x");
    Serial.println(readAddress(0x04, 1), HEX);
    Serial.print("0 0x06: 0x");
    Serial.println(readAddress(0x06, 0), HEX);
    writeByte(0x06, 0x00, 0);
    Serial.print("0 0x06: 0x");
    Serial.println(readAddress(0x06, 0), HEX);
    Serial.print("0 0x06: 0x");
    Serial.println(readAddress(0x06, 0), HEX);
    writeByte(0x04, 0x81, 0);


}


void Zetaohm_MAX7301::setSPIspeed(uint32_t spispeed){
	_spiTransactionsSpeed = spispeed;
}


void Zetaohm_MAX7301::writeByte(byte addr, byte data, bool chipNum){
	startTransaction();
	digitalWriteFast(_cs, LOW);

  if (!chipNum){ //chipnum == 0, targeting the first chip in the MOSI chain
    //clock in command bits to first chip
    SPI.transfer(addr & 0x7F);
  	SPI.transfer(data);
    // toggle CS to very transaction received.

  } else {// chipNum == 1, targeting the second chip in the mosi chain
    SPI.transfer(addr & 0x7F);
    SPI.transfer(data);
    //send no-op to push command to second chip
    SPI.transfer(0x00);
  	SPI.transfer(0x00);
  }

	digitalWriteFast(_cs, HIGH);
	endTransaction();
}

uint8_t Zetaohm_MAX7301::readAddress(byte addr, bool chipNum){
	waitFifoEmpty();
	startTransaction();
	digitalWriteFast(_cs, LOW);

  byte low_byte;
  byte high_byte;

  if (!chipNum){ //chipnum == 0, targeting the first chip in the MOSI chain
    // clock in no-ops for the second chip
    SPI.transfer(0x00);
  	SPI.transfer(0x00);
    //clock in command bits to first chip
    SPI.transfer( addr | 0x80);
  	SPI.transfer(0x00);
    // toggle CS to very transaction received.
    digitalWriteFast(_cs, HIGH);
    digitalWriteFast(_cs, LOW);

    //send 2x16bits of no-ops to flush response
    SPI.transfer(0x00);
  	SPI.transfer(0x00);

    low_byte  = SPI.transfer(0x00);
  	high_byte = SPI.transfer(0x00);

  } else {// chipNum == 1, targeting the second chip in the mosi chain
    SPI.transfer( addr | 0x80);
  	SPI.transfer(0x00);
    //send no-op to push command to second chip
    SPI.transfer(0x00);
  	SPI.transfer(0x00);

    digitalWriteFast(_cs, HIGH);
  	digitalWriteFast(_cs, LOW);

    low_byte  = SPI.transfer(0x00);
  	high_byte = SPI.transfer(0x00);

    SPI.transfer(0x00);
  	SPI.transfer(0x00);
  }

	digitalWriteFast(_cs, HIGH);
	endTransaction();

  uint16_t temp = low_byte | (high_byte << 8);

	return high_byte;
}


void Zetaohm_MAX7301::gpioPinMode(uint16_t mode, bool chipNum){
	if(mode == INPUT){
  		writeByte(0x09, 0xAA, chipNum);
  		writeByte(0x0A, 0xAA, chipNum);
  		writeByte(0x0B, 0xAA, chipNum);
  		writeByte(0x0C, 0xAA, chipNum);
  		writeByte(0x0D, 0xAA, chipNum);
  		writeByte(0x0E, 0xAA, chipNum);
  		writeByte(0x0F, 0xAA, chipNum);
	} else if (mode == OUTPUT) {
  		writeByte(0x09, 0x55, chipNum);
  		writeByte(0x0A, 0x55, chipNum);
  		writeByte(0x0B, 0x55, chipNum);
  		writeByte(0x0C, 0x55, chipNum);
  		writeByte(0x0D, 0x55, chipNum);
  		writeByte(0x0E, 0x55, chipNum);
  		writeByte(0x0F, 0x55, chipNum);
	} else { // INPUT_PULLUP
  		writeByte(0x09, 0xFF, chipNum);
  		writeByte(0x0A, 0xFF, chipNum);
  		writeByte(0x0B, 0xFF, chipNum);
  		writeByte(0x0C, 0xFF, chipNum);
  		writeByte(0x0D, 0xFF, chipNum);
  		writeByte(0x0E, 0xFF, chipNum);
  		writeByte(0x0F, 0xFF, chipNum);
	}

}

void Zetaohm_MAX7301::update(){
	// load the 32 bit integer with the status of all buttons.
	uint32_t previousState = inputBuffer;

	if (debounceTimer > DEBOUNCE_THRESHOLD){
		inputBuffer =
			~( (readAddress(0x44, 0) )
			| ((readAddress(0x4C, 0) ) << 8 )
			| ((readAddress(0x54, 0) ) << 16 )
			| ((readAddress(0x5C, 0) ) << 24 ) );

		fellBuffer = (inputBuffer & ~previousState) | fellBuffer;
		roseBuffer = (~inputBuffer & previousState) | roseBuffer;
		if (previousState != inputBuffer){
			//Serial.println("debounce timer: " + String(debounceTimer) );
			debounceTimer = 0;
		}
	}
  Serial.print(String(millis()) + " 0 0x04: 0x");
  Serial.println(readAddress(0x04, 0), HEX);
  Serial.print(String(millis()) + " 0 0x06: 0x");
  Serial.println(readAddress(0x06, 0), HEX);
  writeByte(0x06, 0xFF, 0);

  Serial.print(String(millis()) + " 0 0x06: 0x");
  Serial.println(readAddress(0x06, 0), HEX);
  elapsedMicros writeByteTimer = 0;
  writeByte(0x04, 0x01, 0);
  Serial.print("writebyte timer: ");
  Serial.println(String(writeByteTimer));

  writeByteTimer = 0;
  writeByte(0x04, 0x81, 0);
  Serial.print("writebyte timer: ");
  Serial.println(String(writeByteTimer));

};

void Zetaohm_MAX7301::maxWrite(bool chipNum, uint8_t pin, bool state){
  writeByte(0x20 | pin, state , chipNum);
  Serial.print("trying to set pin: ");
  Serial.print(0x20 | pin, HEX);
  Serial.println(" to " + String(state));
  Serial.println(readAddress(0x20 | pin, chipNum), HEX) ;
}

void Zetaohm_MAX7301::init(uint8_t index, uint8_t pin) {
	indexMap[index] = pin;
};

bool Zetaohm_MAX7301::fell(uint8_t index){
	if ( fellBuffer & (1 << indexMap[index]) ) {
		fellBuffer = fellBuffer & ~(1 << indexMap[index]) ;
		return true;
	} else {
		return false;
	}
};

bool Zetaohm_MAX7301::rose(uint8_t index){
	if ( roseBuffer & (1 << indexMap[index]) ) {
		roseBuffer = roseBuffer & ~(1 << indexMap[index]) ;
		return true;
	} else {
		return false;
	}
};

bool Zetaohm_MAX7301::pressed(uint8_t index){
	if (inputBuffer & (1 << indexMap[index])){
		return true;
	} else {
		return false;
	}
};
