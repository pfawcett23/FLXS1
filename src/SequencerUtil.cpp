#include "Arduino.h"
#include "Sequencer.h"

void Sequencer::initialize(uint8_t ch, uint8_t stepCount, uint8_t beatCount, uint32_t tempoX100, OutputController* outputControl){
	// initialization routine that runs during setup
	Serial.println("Initializing Sequencer Object");
	this->channel = ch;
	this->pattern = 0;
	this->stepCount = stepCount;
	this->beatCount = beatCount;
  this->clockDivision = 4;

	this->tempoX100 = tempoX100;
	//this->beatLength = 60000000/(tempoX100/100);
	this->calculateStepTimers();
	this->monophonic = true;
	this->outputControl = outputControl;
};


void Sequencer::initNewSequence(uint8_t pattern, uint8_t ch){
	Serial.println("*&*&*&*& Initializing pattern: " + String(pattern) + " channel: " + String(ch));
	this->stepCount 				= 16;
	this->beatCount 				= 4;
	this->quantizeKey 			= 0;
	this->quantizeScale 		= 0;
	this->pattern 					= pattern;
	this->channel 					= ch;
	this->avgPulseLength 		= 20000;
  this->clockDivision 		= 4;
	this->gpio_reset 				= 5;
	this->gpio_yaxis 				= 5;
	this->cv_arptypemod 			  = 5;
	this->gpio_gateinvert 	= 5;
	this->gpio_randompitch 	= 5;
	this->cv_arpspdmod 			= 5;
	this->cv_arpoctmod 			= 5;
	this->cv_arpintmod 			= 5;
	this->cv_pitchmod 			= 5;
	this->cv_gatemod 				= 5;
	this->cv_glidemod 			= 5;

	for(int n=0; n < MAX_STEPS_PER_SEQUENCE; n++){
		this->stepData[n].pitch[0]   = 24;
		for (int i=1; i<4; i++){
			this->stepData[n].pitch[i] = 0;
		}
		this->stepData[n].chord	   		 =	 0;
		this->stepData[n].gateType		 =	 0;
		this->stepData[n].gateLength	 =	 3;
		this->stepData[n].arpType			 =	 0;
		this->stepData[n].arpOctave		 =   1;
		this->stepData[n].arpSpdNum		 =   1;
		this->stepData[n].arpSpdDen		 =   4;
		this->stepData[n].glide				 =   0;
		this->stepData[n].beatDiv			 =   4;
		this->stepData[n].velocity		 =  67;
		this->stepData[n].velocityType =   0;
		this->stepData[n].lfoSpeed		 =  16;
		this->stepData[n].offset       =   0;
		this->stepData[n].noteStatus   =  AWAITING_TRIGGER;
		this->stepData[n].arpStatus   =   0;
		this->stepData[n].notePlaying  =   0;
		this->stepData[n].framesRemaining = 0;
	}
};


void Sequencer::setTempo(uint32_t tempoX100){
	this->tempoX100 = tempoX100;
	//beatLength = 60000000	/(tempoX100/100);
	calculateStepTimers();
}

void Sequencer::setStepPitch(uint8_t step, uint8_t pitch, uint8_t index){
	stepData[step].pitch[index] = pitch;
	Serial.println("step: " + String(step) + " pitch: " + String(pitch) + " index: " + String(index) + " set pitch: " + String(stepData[step].pitch[index]));
};

void Sequencer::setGateLength(uint8_t step, uint8_t length){
	stepData[step].gateLength = length;
	calculateStepTimers();
};

void Sequencer::setStepCount(uint8_t stepCountNew){
	//stepCount = stepCountNew;
	calculateStepTimers();
};

void Sequencer::setBeatCount(uint16_t beatCountNew){
	beatCount = beatCountNew;
	calculateStepTimers();
};

void Sequencer::setGateType(uint8_t step, uint8_t gate){
	stepData[step].gateType = gate;
}

void Sequencer::setStepVelocity(uint8_t step, uint8_t velocity){
	stepData[step].velocity = velocity;
};

void Sequencer::setStepGlide(uint8_t step, uint8_t glideTime){
	stepData[step].glide = glideTime;
}



uint8_t Sequencer::quantizePitch(uint8_t note, uint8_t key, uint8_t scale, bool direction){

	uint8_t count = 0;
	uint16_t scaleExpanded;

	switch(scale){
		case 1: scaleExpanded = CHROMATIC         ; break;
		case 2: scaleExpanded = MAJOR             ; break;
		case 3: scaleExpanded = MINOR             ; break;
		case 4: scaleExpanded = MAJORMINOR        ; break;
		case 5: scaleExpanded = PENTATONIC_MAJOR  ; break;
		case 6: scaleExpanded = PENTATONIC_MINOR  ; break;
		case 7: scaleExpanded = PENTATONIC_BLUES  ; break;
		case 8: scaleExpanded = IONIAN            ; break;
		case 9: scaleExpanded = AEOLIAN           ; break;
		case 10: scaleExpanded = DORIAN           ; break;
		case 11: scaleExpanded = MIXOLYDIAN       ; break;
		case 12: scaleExpanded = PHRYGIAN         ; break;
		case 13: scaleExpanded = LYDIAN           ; break;
		case 14: scaleExpanded = LOCRIAN          ; break;
		default: scaleExpanded = CHROMATIC				; break;
	}

	//Serial.println("Original Scale:\t" + String(scaleExpanded, BIN) );
	for (int i=0; i< key; i++){
		//bitwise rotation - 11 bits rotate to the right. Do it once for each scale degree
		scaleExpanded = (scaleExpanded >> 1) | ((0b01 & scaleExpanded) << 11);
	}
	//Serial.println("Shifted to " + String(quantizeKey) + "\t" + String(scaleExpanded, BIN) );

	while ( (0b100000000000 >> (note % 12) ) & ~scaleExpanded ) {
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
	return note;
}

uint8_t Sequencer::getStepPitch(uint8_t step, uint8_t index){
	return stepData[step].pitch[index];
};

int Sequencer::positive_modulo(int i, int n) {
	return (i % n + n) % n;
}


void Sequencer::gateInputTrigger(uint8_t inputNum){
  if (gpio_reset == inputNum){
    this->clockReset(true);
  }

  if(gpio_yaxis == inputNum){
		if(playing){
			this->skipStep(4);
		}
  }


};

void Sequencer::skipStep(uint8_t count){
  Serial.println("skipStep: " + String(count) + "\tppqPulseIndex: "+ String(ppqPulseIndex) + "\tpulsesPerBeat: " + String(pulsesPerBeat) + "\t" );

  uint16_t oldPpqPulseIndex = ppqPulseIndex;
  ppqPulseIndex = (ppqPulseIndex + count*pulsesPerBeat/clockDivision) % (pulsesPerBeat*stepCount/clockDivision );
  for(int stepNum = 0; stepNum < stepCount; stepNum++){
    stepData[stepNum].framesRemaining += oldPpqPulseIndex-ppqPulseIndex;
  }
}


void Sequencer::noteTrigger(uint8_t stepNum, bool gateTrig, uint8_t arpTypeTrig, uint8_t arpOctaveTrig){
	uint8_t pitchArray[22];
	pitchArray[0] = stepData[stepNum].pitch[0];
	pitchArray[1] = stepData[stepNum].pitch[0] + stepData[stepNum].pitch[1];
	pitchArray[2] = stepData[stepNum].pitch[0] + stepData[stepNum].pitch[2];
	pitchArray[3] = stepData[stepNum].pitch[0] + stepData[stepNum].pitch[3];

	//figure out how many steps are nil (255)
	uint8_t arpSteps = 4;
	for(int i=1; i<4; i++){
		if(stepData[stepNum].pitch[i] == 255){
			arpSteps = arpSteps - 1;
		}
	}

	//fill up the rest of the array with octave up pitches
	for (int i = arpSteps; i< 5 * arpSteps; i++){
		pitchArray[i] = pitchArray[i-arpSteps]+12;
	}

	if (arpOctaveTrig > 0){
		arpSteps = arpOctaveTrig * arpSteps;
	}

	uint8_t index;

	int8_t playPitch; 	//pitch that will be triggered in this loop

	switch (arpTypeTrig){
		case ARPTYPE_UP:
			index = stepData[stepNum].arpStatus % arpSteps;
			playPitch = pitchArray[index];
		break;

		case ARPTYPE_DN:
			index = arpSteps - stepData[stepNum].arpStatus % arpSteps;
			playPitch = pitchArray[index];
		break;

		case ARPTYPE_UPDN1:
			// repeating top and bottom note
			if (stepData[stepNum].arpStatus/arpSteps % 2){
				index = stepData[stepNum].arpStatus % arpSteps;
			} else {
				index = arpSteps - stepData[stepNum].arpStatus % arpSteps;
			}

			playPitch = pitchArray[index];
		break;

		case ARPTYPE_UPDN2: // no repeat of top and bottom note
	//arpsteps:           4
	//index:              0 1 2 3 2 1 0 1 2 3
	//arpStatus:          0 1 2 3 4 5 6 7 8 9
	//arpStatus/arpSteps: 0 0 0 0 1 1 1 1 2 2
	//arpstatus%arpSteps: 0 1 2 3 0 1 2 3 0 1
	//                    0 0 0 0 1 1 1
	//										0 0 0 1 1 1 0 0 0 1 1 1
	//up:                 0 1 2 3 0 1 2 3 0 1 2 3
	//down:      			  3 2 1 0 3 2 1 0 3 2

			if (stepData[stepNum].arpStatus/(arpSteps-1) % 2){
				index = stepData[stepNum].arpStatus % arpSteps;
			} else {
				index = arpSteps - stepData[stepNum].arpStatus % (arpSteps-1);
			}
			playPitch = pitchArray[index];
		break;

		case ARPTYPE_RNDM: // no repeat of top and bottom note
			index = random(0,arpSteps);
			playPitch = pitchArray[index];
		break;

		default:
			playPitch = pitchArray[0];
		break;
	}

	//if (quantizeKey == 1){
	//	stepData[stepNum].notePlaying = quantizePitch(playPitch, aminor, 1);
	if (quantizeScale > 0){
		stepData[stepNum].notePlaying = quantizePitch(playPitch, quantizeKey, quantizeScale, 1);
	} else {
		stepData[stepNum].notePlaying = playPitch;
	}
	//Serial.println("noteOn"); delay(10);

	//BEGIN INPUT MAPPING SECTION
	if (gateInputRaw[gpio_randompitch]){
		stepData[stepNum].notePlaying = random(0, 127);
	}

	stepData[stepNum].notePlaying += cvInputMapped[cv_pitchmod]/4;
	uint8_t glideVal = min_max(stepData[stepNum].glide + cvInputMapped[cv_glidemod], 0, 255);

	if(gateInputRaw[gpio_gateinvert]){
		gateTrig = !gateTrig;
	}

	if(stepData[stepNum].arpType == ARPTYPE_OFF){
		// THIS INPUT MAPPING STILL NEEDS WORK.
		// CUTS NOTES OFF WHEN GATE IS TOO LONG.
		// NEED TO ADD WATCHDOG TO TURN NOTES OFF BEFORE A NEW ONE IS TRIGGERED
		stepData[stepNum].framesRemaining = min_max(2*stepData[stepNum].gateLength+2 + cvInputMapped[cv_gatemod], 1, 64 ) * FRAMES_PER_BEAT / (8*clockDivision);
		stepData[stepNum].arpLastFrame =  FRAMES_PER_BEAT / (8 * clockDivision);
		//Serial.println("Setting note with gate length: " + String(min_max(2*stepData[stepNum].gateLength+2 + cvInputMapped[cv_gatemod], 1, 256 ))  + "\tframesremaining; " + String(stepData[stepNum].framesRemaining) + "\tarplastframe: " + String(stepData[stepNum].arpLastFrame) + "\tSL: " + String(getStepLength()));


	} else {
		stepData[stepNum].framesRemaining = FRAMES_PER_BEAT *  getArpSpeedNumerator(stepNum) / (clockDivision * getArpSpeedDenominator(stepNum));
		stepData[stepNum].arpLastFrame = stepData[stepNum].framesRemaining/2;

		//Serial.println("Setting note with gate length:\tframesremaining; " + String(stepData[stepNum].framesRemaining) + "\tarplastframe: " + String(stepData[stepNum].arpLastFrame) + "\tSL: " + String(getStepLength()));

	}

  DEBUG_PRINT("clockDiv:" + String(clockDiv) + "arpLastFrame: " + String(stepData[stepNum].arpLastFrame));

	//END INPUT MAPPING SECTION
	outputControl->noteOn(channel,stepData[stepNum].notePlaying,stepData[stepNum].velocity,stepData[stepNum].velocityType, stepData[stepNum].lfoSpeed, glideVal, gateTrig );
	stepData[stepNum].arpStatus++;

	//ensuring that gate is turned off before next step:
	// uint8_t nextStep = stepCount;
	// for(int nxtStp = stepNum + 1; nxtStp < stepCount; nxtStp++){
	// 	if (stepData[nxtStp].gateType != GATETYPE_REST){
	// 		nextStep = nxtStp;
	// 		break;
	// 	}
	// }


	//stepData[stepNum].framesRemaining = std::min( (int)stepData[stepNum].framesRemaining ,(int) (nextStep*FRAMES_PER_BEAT/clockDivision - getCurrentFrame() - FRAMES_PER_BEAT/16) );

}

uint8_t  Sequencer::getArpSpeedNumerator(uint8_t stepNum){
	if (arpSpeedModulation[stepNum] < 0){
		return min_max(stepData[stepNum].arpSpdNum - abs(arpSpeedModulation[stepNum]), 1, 64) ;
	} else {
		return stepData[stepNum].arpSpdNum;
	}
};
uint8_t  Sequencer::getArpSpeedDenominator(uint8_t stepNum){
	if (arpSpeedModulation[stepNum] > 0){
		return stepData[stepNum].arpSpdDen * arpSpeedModulation[stepNum];
	} else {
		return stepData[stepNum].arpSpdDen;
	}
};

uint8_t Sequencer::getArpCount(uint8_t stepNum){
	uint8_t arpCount;

	if (arpTypeModulated[stepNum] == ARPTYPE_OFF) {
		arpCount = 0;
	} else {
		arpCount = (stepData[stepNum].gateLength *  getArpSpeedDenominator( stepNum) / getArpSpeedNumerator(stepNum))/4 ;
	}

	return arpCount;
}

/*void Sequencer::clearNoteData(NoteDatum *noteData){
	noteData->noteOff = false;
	noteData->noteOn = false;

	for(int i = 0; i < stepCount; i++){
		noteData->noteOnArray[i] = NULL;
		noteData->noteVelArray[i] = NULL;
		noteData->noteVelTypeArray[i] = NULL;
		noteData->noteLfoSpeed[i] = NULL;
		noteData->noteGlideArray[i] = NULL;
		noteData->noteOffArray[i] = NULL;
	}

	noteData->channel = 0;
	noteData->noteOnStep = 0;
	noteData->noteOffStep = 0;
}*/

void Sequencer::noteShutOff(uint8_t stepNum, bool gateOff){
	//shut off any other notes that might still be playing.
//	Serial.println("shutting step " + String(stepNum) + " off");
		if( stepData[stepNum].noteStatus == CURRENTLY_PLAYING ){
	//		noteData->noteOff = true;
	//		noteData->channel = channel;
	//		noteData->noteOffStep = stepNum;
	//		noteData->noteGateOffArray[stepNum] = gateOff;


			outputControl->noteOff(channel, stepData[stepNum].notePlaying, gateOff );
/*
			for (int f=0; f<stepCount; f++){
				if (noteData->noteOffArray[f] == NULL){
					noteData->noteOffArray[f] = stepData[stepNum].notePlaying;
					break;
				}
			}
*/
		}

}