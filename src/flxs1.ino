/* **********************************
*** ZETAOHM FLXS1 MUSIC SEQUENCER ***
********************************** */
#include <Arduino.h>
#include <SPI.h>
#include <i2c_t3.h>
#include "TimeController.h"
#include "Sequencer.h"

#include "global.h"
#include "config.h"
//#include "InputModule.h"
#include "DisplayModule.h"
//#include "midiModule.h"
//#include "LEDArray.h"

TimeController timeControl;
IntervalTimer MasterClockTimer;

void setup() {
  Serial.begin(kSerialSpeed);
  //waiting for serial to begin
  delay(1500);
  Serial.println("<<<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>> Setup <<<<----===---==--=-|*+~^~+*|-=--==---===---->>>>>");

  SPI.begin();
  SPI.setMOSI(kSpiMosiPin);
  SPI.setMISO(kSpiMisoPin);
	SPI.setSCK(kSpiClockPin);

  timeControl.initialize();
//	MasterClockTimer.begin(masterLoop,kClockInterval);
//	SPI.usingInterrupt(MasterClockTimer);
  Serial.println("<<<--||-->>> Setup Complete <<<--||-->>>");

}

void loop() {
  timeControl.runLoopHandler();
}

void masterLoop(){
  timeControl.masterClockHandler();
}
