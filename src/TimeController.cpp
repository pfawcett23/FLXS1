#include <Arduino.h>
#include "TimeController.h"

#define DISPLAY_FREQUENCY 10000
#define INPUT_FREQUENCY 200000
#define LED_FREQUENCY 30000

TimeController::TimeController(){};

void TimeController::initialize() {
	Serial.println("Initializing TimeController");

	pinMode(14, OUTPUT);
	digitalWrite(14, LOW);
	digitalWrite(14, HIGH);
	digitalWrite(14, LOW);

	sequence[0].initialize(0, 16, 4, (tempoX100/100));
  sequence[1].initialize(1, 16, 4, (tempoX100/100));
  sequence[2].initialize(2, 16, 4, (tempoX100/100));
  sequence[3].initialize(3, 16, 4, (tempoX100/100));

	outputControl.initialize();
	ledArray.initialize(sequence);
	display.initialize(sequence);
	digitalWrite(14, HIGH);

	digitalWrite(14, LOW);
	buttonIo.initialize(&outputControl, &saveFile, sequence);
	digitalWrite(14, HIGH);
	buttonIo.changeState(STEP_DISPLAY);
	//clockMaster.initialize(&outputControl, sequence);
	//saveFile.initialize(sequence);
	//saveFile.loadPattern(0, 0b1111);

/*
	saveFile.saveSequenceJSON(sequence[0], 0, 0 );
	delay(200);
	saveFile.readSequenceJSON(sequence[0], 0, 0);
	File root = SD.open("/");
  saveFile.printDirectory(root, 2);
	*/
}

void TimeController::runLoopHandler() {
	//ledArray.loop(LED_FREQUENCY);
	digitalWrite(14, LOW);
	buttonIo.loop(INPUT_FREQUENCY);
	digitalWrite(14, HIGH);
	//display.displayLoop(DISPLAY_FREQUENCY);

	//for (int j=0; j< 128; j++){
		for (int i = 0; i<4;i++){
		//	Serial.println("switching notes:" + String(i));
			outputControl.noteOn(i,127,127);
			outputControl.noteOff(i,127);
		}
		delay(10);
		for (int i = 0; i<4;i++){

			outputControl.noteOn(i,0,0);
			outputControl.noteOff(i,0);
		}
		delay(10);

	//}


}

void TimeController::masterClockHandler(){
	clockMaster.masterClockFunc();
}
