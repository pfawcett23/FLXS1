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
#include "global.h"

#ifndef _InputModule_h_
#define _InputModule_h_
//button stuff
#define SW_00    0
#define SW_01    1
#define SW_02    2
#define SW_03    3
#define SW_04    4
#define SW_05    5
#define SW_06    6
#define SW_07    7
#define SW_08    8
#define SW_09    9
#define SW_10    10
#define SW_11    11
#define SW_12    12
#define SW_13    13
#define SW_14    14
#define SW_15    15
#define SW_PLAY  16
#define SW_REC   17
#define SW_STOP  18
#define SW_M0    19
#define SW_M1    20
#define SW_M2    21
#define SW_M3    22
#define SW_PGDN  23
#define SW_PGUP  24
#define SW_PATTERN  25
#define SW_MENU   26
#define SW_SHIFT   27
#define ENCODER1LEFTPIN 16
#define ENCODER1RIGHTPIN 25 
#define MAX7301PIN  5

class InputModule
{
public:
  InputModule();
  Encoder knob;
  Zetaohm_MAX7301* midplaneGPIO;
  Zetaohm_MAX7301* backplaneGPIO;
  OutputController* outputControl;
  MasterClock* clockMaster;

  void initialize(OutputController* outputControl, Zetaohm_MAX7301* midplaneGPIO, Zetaohm_MAX7301* backplaneGPIO, FlashMemory* saveFile, Sequencer *sequenceArray, MasterClock* clockMaster);

  void loop(uint16_t frequency);

  void patternSelectHandler();
  //void channelMenuHandler();
  void channelButtonHandler(uint8_t channel);
  void channelButtonShiftHandler(uint8_t channel);
  void altButtonHandler();
  void stepModeMatrixHandler();

  void channelPitchModeInputHandler();
  void channelVelocityModeInputHandler();
  void channelEnvelopeModeInputHandler();
  void channelStepModeInputHandler();


  void sequenceMenuHandler();
  void globalMenuHandler();
  void tempoMenuHandler();

  void timingMenuInputHandler();
  void debugScreenInputHandler();
  void calibrationMenuHandler();

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