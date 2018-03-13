#include <Arduino.h>
#include "globalVariable.h"

void GlobalVariable::initialize(ADC *adc){
      this->adc = adc;

      this->clockMode = INTERNAL_CLOCK;
      this->multi_pitch_switch = 0;
      this->multi_arpInterval_switch = 0;
      this->multi_gateType_switch = 0;
      this->multi_gateLength_switch = 0;
      this->multi_arpType_switch = 0;
      this->multi_arpOctave_switch = 0;
      this->multi_arpSpdNum_switch = 0;
      this->multi_arpSpdDen_switch = 0;
      this->multi_glide_switch = 0;
      this->multi_beatDiv_switch = 0;
      this->multi_velocity_switch = 0;
      this->multi_velocityType_switch = 0;
      this->multi_cv2speed_switch = 0;
      this->multi_cv2offset_switch = 0;
      this->multi_pitch = 0;
      this->multi_arpInterval = 0;
      this->multi_gateType = 0;
      this->multi_gateLength = 0;
      this->multi_arpType = 0;
      this->multi_arpOctave = 0;
      this->multi_arpSpdNum = 0;
      this->multi_arpSpdDen = 0;
      this->multi_glide = 0;
      this->multi_beatDiv = 0;
      this->multi_velocity = 0;
      this->multi_velocityType = 0;
      this->multi_cv2speed = 0;
      this->multi_cv2offset = 0;
      this->muteChannelSelect[4] = {false};
      this->randomizeParamSelect = 0;
      this->randomizeLow = 36;
      this->randomizeSpan = 3;

      for(int i = 0; i < 64; i++){
          this->multiSelection[i]=false;
      }
      for(int i = 0; i < 4; i++){
        this->gateTestArray[i]=255;
      }
      activeGate = 0;
      rheoTestLevel = 0;
      gateTestComplete = false;

      this->parameterSelect = false;
      this->midiSetClockOut = false;
      this->calibrationBuffer = 0;
      this->stepCopyIndex = 255;
      this->chCopyIndex = 255;

       for(int i=0; i<8; i++){ this->dacCalibrationNeg[i]= 0; };
       for(int i=0; i<8; i++){ this->dacCalibrationPos[i]= 65535; };
       for(int i=0; i<4; i++){ this->adcCalibrationPos[i]= 0; };
       for(int i=0; i<4; i++){ this->adcCalibrationNeg[i]= 65535; };
       for(int i=0; i<4; i++){ this->adcCalibrationOffset[i]= 32767; };

       this->initGlobals();
    }

void GlobalVariable::initGlobals(){
  this->dataInputStyle = 0;
  this->pageButtonStyle = 1;
  this->outputNegOffset[0] = 2;
  this->outputNegOffset[1] = 2;
  this->outputNegOffset[2] = 2;
  this->outputNegOffset[3] = 2;
}

bool GlobalVariable::extClock(){
  if(clockMode >= EXTERNAL_CLOCK_GATE_0){
    return true;
  } else {
    return false;
  }
}

int GlobalVariable::generateRandomNumber(int minVal, int maxVal){
  uint32_t randomNum;

    randomNum = (adc->analogRead(A6)&0b1111);
    randomNum |= ((adc->analogRead(A7)&0b1111) << 4);
    randomNum |= ((adc->analogRead(A6)&0b1111) << 8);
    randomNum |= ((adc->analogRead(A7)&0b1111) << 12);
    randomNum |= ((adc->analogRead(A6)&0b1111) << 16);
    randomNum |= ((adc->analogRead(A7)&0b1111) << 20);
    randomNum |= ((adc->analogRead(A6)&0b1111) << 24);
    randomNum |= ((adc->analogRead(A7)&0b1111) << 28);
    randomNum ^= ARM_DWT_CYCCNT;

    srand(randomNum);
    randomNum = (rand()%(maxVal-minVal))+minVal;

  return randomNum;
}

int16_t GlobalVariable::quantizeSemitonePitch(int16_t note, uint8_t quantizeKey, uint16_t quantizeMode, bool direction){
  uint8_t count = 0;
  uint16_t scaleExpanded = quantizeMode;
  uint8_t scaleDivisions = 12;
  scaleDivisions -= 1; //minus one for the octave. 11 notes in a 12-tet scale
  //Serial.print("note: " + String(note) + "\tquantizeMode: " + String(quantizeMode, BIN));
  for (int i=0; i< quantizeKey; i++){
		//bitwise rotation - 11 bits rotate to the right. Do it once for each scale degree
    scaleExpanded = (scaleExpanded << 1) | ((0b01 << scaleDivisions) & scaleExpanded) >> scaleDivisions;
	}

	while ( (1 << (note % 12) ) & ~scaleExpanded ) {
		if (direction){
			note += 1;
		} else {
			note -= 1;
		}
		count += 1;
		if (count > 12) {
			break; // emergency break if while loop goes OOC
		}
	}
  //Serial.println("\tScaleExpanded: " + String(scaleExpanded, BIN) + "\tquantizedNote: " + String(note) + "\tcount: " + String(count));
  return note;
}
