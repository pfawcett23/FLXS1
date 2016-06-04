#include <Arduino.h>
#include "midiModule.h"
/*
void midiSetup(){
//  MasterClockMemFn midiClockPulseHandlerFp = &MasterClock::midiClockPulseHandler;
///  MasterClockMemFn midiStartContinueHandlerFp = &MasterClock::midiStartContinueHandler;

//  MIDI.begin(MIDI_CHANNEL_OMNI);
//  MIDI.setHandleClock( (clockMaster->*midiClockPulseHandlerFp) );
//  MIDI.setHandleNoteOn(midiNoteOnHandler);
//  MIDI.setHandleNoteOff(midiNoteOffHandler);
//  MIDI.setHandleStart( (clockMaster->*midiStartContinueHandlerFp) );
//  MIDI.setHandleContinue( (clockMaster->*midiStartContinueHandlerFp) );
//  MIDI.setHandleStop(midiStopHandler);
}

void midiStopHandler(){
  playing = 0;
}

void midiNoteOffHandler(byte channel, byte note, byte velocity){
  Serial.println(String("Note Off: ch=") + channel + ", note=" + note + ", velocity=" + velocity);
}

void midiNoteOnHandler(byte channel, byte note, byte velocity){
  if (velocity > 0) {
    Serial.println(String("Note On:  ch=") + channel + ", note=" + note + ", velocity=" + velocity);
  } else {
    Serial.println(String("Note Off: ch=") + channel + ", note=" + note);
  }
}


void midiClockSyncFunc(){
  noInterrupts();
//  MIDI.read();
  interrupts();
}
*/
