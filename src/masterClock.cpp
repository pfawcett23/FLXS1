#include <Arduino.h>
#include "MasterClock.h"

void MasterClock::initialize(OutputController * outputControl, Sequencer (*sequenceArray)[4]){
	Serial.println("Initializing Master Clock");
	this->sequenceArray = sequenceArray;
	this->outputControl = outputControl;
	Serial.println("Master Clock Initialized");

};

void MasterClock::changeTempo(uint32_t newTempoX100){
	tempoX100 = newTempoX100;
	beatLength = 60000000/(tempoX100/100);
  for (int i = 0; i < sequenceCount; i++ ){
    sequenceArray[i]->setTempo(tempoX100);
  }
}

void MasterClock::masterClockFunc(void){
 elapsedMicros loopTimer = 0;

if (inputTimer > 10000){
  inputTimer = 0;
}
//  avgInterval =((micros() - lastMicros) + 9* avgInterval) / 10;
//  timerAvg = (lastTimer + 9*timerAvg) /10;
//  lastMicros = micros();
//
//  intervalJitter = (abs(int(avgInterval) - int(lastAvgInterval)));
//  avgIntervalJitter = (intervalJitter * 9 + avgIntervalJitter) / 10;
//  lastAvgInterval = avgInterval;

  if(playing){

    if( extClock == false ){
      internalClockTick();
    } else {
      externalClockTick();
    }

    noteOffSwitch();

    noteOnSwitch();

  }

  wasPlaying = playing;
  lastTimer = loopTimer;

//if (pixelTimer > 2000){
//  pixelTimer = 0;
//}
//  midiClockSyncFunc();
}


void MasterClock::internalClockTick(){
 //digitalWriteFast(DEBUG_PIN, HIGH);

  debug("begin internal clock tick");
        // int clock
  if (wasPlaying == false){
        // if playing has just re-started, the master tempo timer and the master beat count must be reset
   // MIDI.send(Start, 0, 0, 1);  // MIDI.sendSongPosition(0);
    masterTempoTimer = 0;
    masterPulseCount = 0;
    internalClockTimer = 0;
    startTime = 0;

    for (int i=0; i< sequenceCount; i++){
      sequenceArray[i]->clockStart(startTime);
      sequenceArray[i]->beatPulse(beatLength, &life);
      sequenceArray[i]->runSequence(&noteData[i], &life);
    }
  } else if (internalClockTimer > 60000000/(tempoX100/100)){
       // Serial.print(" b4 ");
    if (queuePattern != currentPattern) {
      //changePattern(queuePattern, true, true);
    }
    for (int i=0; i< sequenceCount; i++){
      sequenceArray[i]->runSequence(&noteData[i], &life);
      sequenceArray[i]->beatPulse(beatLength, &life);
    }
    tempoBlip = !tempoBlip;
    internalClockTimer = 0;

       // Serial.print(" b5 ");
  }  else {
    for (int i=0; i< sequenceCount; i++){
      sequenceArray[i]->runSequence(&noteData[i], &life);
    }
  }
    debug("end internal clock tick");
  //digitalWriteFast(DEBUG_PIN, LOW);

}

void MasterClock::externalClockTick(){
  // ext clock sync
  for (int i=0; i< sequenceCount; i++){
    sequenceArray[i]->runSequence(&noteData[i], &life);
  }
}

void MasterClock::noteOffSwitch(){
  for (int i=0; i< sequenceCount; i++){
    if (noteData[i].noteOff == true){
      for (int n=0; n< 16; n++){
        if (!noteData[i].noteOffArray[n]){
          continue;
        }
				outputControl->noteOff(noteData[i].channel, noteData[i].noteOffArray[n]);
      }
    }
  }
}

void MasterClock::noteOnSwitch(){
    debug( "\tbegin note on switch");

  for (int i=0; i< sequenceCount; i++){
    if (noteData[i].noteOn == true){
      for (int n=0; n< 128; n++){
        if (!noteData[i].noteOnArray[n]){
          continue;
        }
				outputControl->noteOn(noteData[i].channel,noteData[i].noteOnArray[n],noteData[i].noteVelArray[n] );
      }
    }
  }
      debug("end note on switch");

}

void MasterClock::midiClockPulseHandler(){

  if (extClock == true) {
    //Serial.println(" bpm:" + String(bpm)  + "\tavg: " + Sturing(avgBpm) );
    //20bpm = 124825/1
    //120bpm = 20831/1

    // 1 beat = 24 puleses
    // = 24* avg pulse = 24*(masterTempoTimer/(masterPulseCount+1))
    // = 24*(20831/1) = 499944 microseconds per beat /1000000 = .4999 seconds per beat
    if (firstRun){
        firstRun = false;
    }
    //Serial.print(" D5 ");

    masterPulseCount = (masterPulseCount + 1) % 24;

    if (masterPulseCount == 0){
      if (queuePattern != currentPattern) {
//          changePattern(queuePattern, true, true);
      }

      beatLength = masterTempoTimer;
      masterTempoTimer = 0;
      tempoBlip = !tempoBlip;
      blipTimer = 0;
      for (int i=0; i< sequenceCount; i++){
        sequenceArray[i]->beatPulse(beatLength, &life);
      }

      lastBeatLength = beatLength;

    }
    //MasterClockFunc();
  }
}

void MasterClock::midiStartContinueHandler(){
  if (extClock == true) {
    testTimer = 0;
    playing = 1;
    tempoBlip = !tempoBlip;
    blipTimer = 0;
    firstRun = true;
    startTime = 0;
    masterPulseCount = 0;
    masterTempoTimer = 0;
    for (int i=0; i< sequenceCount; i++){
      sequenceArray[i]->clockStart(startTime);
      sequenceArray[i]->beatPulse(beatLength, &life);
    }

    Serial.println("Midi Start / Continue");

    pulseTimer = 0;
  }
}
