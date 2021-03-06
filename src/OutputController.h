#ifndef _OUTPUT_CONTROLLER_H_
#define _OUTPUT_CONTROLLER_H_

#include <Arduino.h>
#include <MIDI.h>
#include <ADC.h>

#include "Zetaohm_MAX7301/Zetaohm_MAX7301.h"
#include "Zetaohm_AD5676/Zetaohm_AD5676.h"
#include "Zetaohm_MCP4352/Zetaohm_MCP4352.h"
#include "FlashMemory.h"
#include "midiModule.h"
#include "globalVariable.h"
#include "global.h"

#define PROGRAM_CHANGE 1
#define SET_CHANNEL_VOLUME 2
#define SET_CHANNEL_BANK 3

#define GATEMAP 0
#define CVMAP 1
#define CVRHEO 2
#define CCMAP 3
#define CCRHEO 4
#define RHEOCHANNELCV 5
#define RHEOCHANNELCC 6
#define SLEWSWITCHCV 7
#define SLEWSWITCHCC 8

class OutputController
{
public:
  void initialize(Zetaohm_MAX7301 *backplaneGPIO, midi::MidiInterface<HardwareSerial> *serialMidi, ADC *adc, GlobalVariable *globalObj, Sequencer *sequenceArray, FlashMemory *saveFile);
  void noteOn(uint8_t channel, uint8_t stepNum, uint16_t note, uint8_t velocity, uint8_t velocityType, uint8_t cv2speedSetting, uint8_t cv2offsetSetting, uint8_t glide, bool gate, bool tieFlag, uint8_t quantizeScale, uint16_t quantizeMode, uint8_t quantizeKey, bool cvMute, uint32_t startFrame, bool notFirstArp);
  void noteOff(uint8_t channel, uint8_t note, bool gateOff);
  uint16_t scale_degree_to_quantized_dac_output(uint8_t channel, uint16_t note, uint8_t negOffset, bool pitchValue);
  void setDacVoltage(uint8_t dac, uint16_t output);
  void cv2settingsChange(uint8_t channel, uint8_t stepNum, uint8_t velocity, uint8_t velocityType, uint8_t cv2speedSetting, uint8_t cv2offsetSetting);
  void cv2update(uint8_t channel, uint32_t currentFrame, uint32_t framesPerSequence, uint32_t stepLength, bool mute);
  void allNotesOff(uint8_t channel);
  void setClockOutput(bool value);
  bool readClockPort();
  void setGateOutputDebug(uint8_t index, bool value);
  bool gpioCheck(int8_t mapValue);

  void clearVelocityOutput(uint8_t channel);
  uint16_t cvInputCheck(uint8_t mapValue);
  uint8_t analogInputTranspose(uint8_t note);

  void flashMemoryLoadPattern(uint8_t pattern, uint8_t channelSelector);
  void dacTestLoop();
  void midiTestLoop();
  void calibrationRoutine();
  void inputLoopTest();
  void inputRead();
  void flashMemoryStaggeredLoadLoop();
  void sequenceArrayResetSequence(uint8_t channel);
  void sysexMessageHandler(char *sysex_message);
  int flashMemoryControl(int value);

  uint16_t calibMidscale(uint8_t mapAddress);
  uint16_t voltageOffset(uint8_t volts, uint8_t mapAddress);

  uint16_t calibZero(uint8_t channel, uint8_t mapAddress, uint8_t negOffset);
  uint16_t calibHigh(uint8_t channel, uint8_t mapAddress, uint8_t negOffset);

  elapsedMillis clockOutputTimer;
  elapsedMillis lfoTimer;

  uint8_t cv2currentStep[4];
  uint8_t cv2type[4];
  uint8_t cv2speed[4];
  int8_t cv2amplitude[4];
  int8_t cv2offset[4];
  uint8_t cv2quantizeKey[4];
  int16_t cv2quantizeMode[4];

  uint32_t lfoStartFrame[4];
  bool sampleAndHoldSwitch[4];
  bool lfoRheoSet[4];
  bool clockValue;

  uint8_t outputMap(uint8_t channel, uint8_t mapType);

  uint32_t debugFrameCount;

private:
  Zetaohm_AD5676 ad5676;
  Zetaohm_MCP4352 mcp4352_1;
  Zetaohm_MCP4352 mcp4352_2;
  ADC *adc;
  Zetaohm_MAX7301 *backplaneGPIO;
  GlobalVariable *globalObj;
  midi::MidiInterface<HardwareSerial> *serialMidi;
  Sequencer *sequenceArray;
  FlashMemory *saveFile;

  uint8_t gateMap[4] = {0, 1, 2, 3};
  uint8_t dacCvMap[4] = {7, 0, 5, 3};
  uint8_t dacCcMap[4] = {1, 6, 2, 4};
  //uint16_t globalObj->adcCalibrationNeg[4] = {65300,65350,65350, 65380 };
  //uint16_t globalObj->adcCalibrationPos[4] = {530,750, 750, 560};
  //uint8_t gateMap[4]  = {4,6,5,7};
  //uint8_t dacCvMap[4] = {2,3,4,5};
  //uint8_t dacCcMap[4] = {1,0,7,6};

  // rheoMap contains corresponding rheostate to the dacCcMap and dacCvMap entries.

  //uint8_t slewSwitchMap[8] = {9,10,11,12,13,14,15,16 };
  uint8_t slewSwitchMap[8] = {9, 10, 16, 14, 13, 15, 11, 12};

  uint8_t rheoMap[8] = {
      0x00 | 3, // 0  // dacchannel
      0x00 | 1, // 1
      0x10 | 2, // 2
      0x10 | 1, // 3
      0x10 | 0, // 4
      0x10 | 3, // 5
      0x00 | 2, // 6
      0x00 | 0  // 7
  };
  const uint16_t semitone10thCent[12] = {0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000};
  const uint16_t ionian10thCent[7] = {0, 2000, 4000, 5000, 7000, 9000, 11000};
  const uint16_t dorian10thCent[7] = {0, 2000, 3000, 5000, 7000, 9000, 10000};
  const uint16_t phyrigan10thCent[7] = {0, 1000, 3000, 5000, 7000, 8000, 10000};
  const uint16_t lydian10thCent[7] = {0, 2000, 4000, 6000, 7000, 9000, 11000};
  const uint16_t mixolydian10thCent[7] = {0, 2000, 4000, 5000, 7000, 9000, 10000};
  const uint16_t aeolian10thCent[7] = {0, 2000, 3000, 5000, 7000, 8000, 10000};
  const uint16_t locrian10thCent[7] = {0, 1000, 3000, 5000, 6000, 8000, 10000};
  const uint16_t bluesMajor10thCent[7] = {0, 3000, 4000, 7000, 9000, 10000};
  const uint16_t bluesMinor10thCent[7] = {0, 3000, 5000, 6000, 7000, 10000};
  const uint16_t pentMajor10thCent[5] = {0, 2000, 4000, 7000, 9000};
  const uint16_t pentMinor10thCent[7] = {0, 3000, 5000, 7000, 10000};
  const uint16_t folk10thCent[8] = {0, 1000, 3000, 4000, 5000, 7000, 8000, 10000};
  const uint16_t japanese10thCent[5] = {0, 1000, 5000, 7000, 8000};
  const uint16_t gamelan10thCent[5] = {0, 1000, 3000, 7000, 8000};
  const uint16_t gypsy10thCent[7] = {0, 2000, 3000, 6000, 7000, 8000, 11000};
  const uint16_t arabian10thCent[7] = {0, 1000, 4000, 5000, 7000, 8000, 11000};
  const uint16_t flamenco10thCent[7] = {0, 1000, 4000, 5000, 7000, 8000, 10000};
  const uint16_t wholetone10thCent[6] = {0, 2000, 4000, 6000, 8000, 10000};
  const uint16_t pythagorean10thCent[12] = {0, 898, 2039, 2938, 4078, 4977, 6117, 7023, 7922, 9062, 9961, 11102};
  const uint8_t semitoneNoteCount = 12;
  const uint8_t ionianNoteCount = 7;
  const uint8_t dorianNoteCount = 7;
  const uint8_t phyriganNoteCount = 7;
  const uint8_t lydianNoteCount = 7;
  const uint8_t mixolydianNoteCount = 7;
  const uint8_t aeolianNoteCount = 7;
  const uint8_t locrianNoteCount = 7;
  const uint8_t bluesMajorNoteCount = 7;
  const uint8_t bluesMinorNoteCount = 7;
  const uint8_t pentMajorNoteCount = 5;
  const uint8_t pentMinorNoteCount = 7;
  const uint8_t folkNoteCount = 8;
  const uint8_t japaneseNoteCount = 5;
  const uint8_t gamelanNoteCount = 5;
  const uint8_t gypsyNoteCount = 7;
  const uint8_t arabianNoteCount = 7;
  const uint8_t flamencoNoteCount = 7;
  const uint8_t wholetoneNoteCount = 6;
  const uint8_t pythagoreanNoteCount = 12;

  /* capctrl  rheonet dacchannel  max7301       backplaneGPIO
    0          3       0          19  p22       9
    1          1       1          20  p23       10
    2          0       6          21  p24       11
    3          2       7          22  p25       12
    4          6       4          23  p26       13
    5          5       3          24  p27       14
    6          4       5          26  p7        15
    7          7       2          25  p28       16
*/
};

#endif
