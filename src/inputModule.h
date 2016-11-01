#include <Arduino.h>
#include <SPI.h>
#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"
#define ENCODER_OPTIMIZE_INTERRUPTS

#include <Encoder.h>
#include "OutputController.h"
#include "DisplayModule.h"
#include "Sequencer.h"
#include "MasterClock.h"
#include "GameOfLife.h"
#include "config.h"
#include "global.h"

#ifndef _InputModule_h_
#define _InputModule_h_
//button stuff
class InputModule
{
public:
  InputModule();
  Encoder knob;
  Zetaohm_MAX7301 max7301;
  OutputController* outputControl;

  void initialize(OutputController* outputControl, FlashMemory* saveFile, Sequencer *sequenceArray);

  void loop(uint32_t frequency);

  void patternSelectHandler();
  void channelMenuHandler();
  void channelButtonHandler(uint8_t channel);
  void altButtonHandler();
  void stepModeMatrixHandler();
  void sequencerMenuHandler();
  void instrumentSelectInputHandler();
  void timingMenuInputHandler();
  void debugScreenInputHandler();
  void resetKnobValues();
  void changeState(uint8_t state);

  // Encoder vars
  int8_t knobRead;
  int8_t knobBuffer;
  int8_t knobPrevious;
  int8_t knobChange;
  int8_t menuSelector;
  int8_t instBuffer;
  int16_t stepModeBuffer;

  unsigned long encoderLoopTime;
  unsigned long smallButtonLoopTime;
  unsigned long encoderButtonTime;
  unsigned long matrixButtonTime;

private:
  Sequencer *sequenceArray;
  FlashMemory *saveFile;
  elapsedMicros inputTimer;

};

#endif
