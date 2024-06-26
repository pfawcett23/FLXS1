#include <Arduino.h>
#include "OutputController.h"

void OutputController::initialize(Zetaohm_MAX7301 *backplaneGPIO, midi::MidiInterface<HardwareSerial> *serialMidi, ADC *adc, GlobalVariable *globalObj, Sequencer *sequenceArray, FlashMemory *saveFile)
{
  Serial.println("Initializing MIDI");
  this->sequenceArray = sequenceArray;
  this->globalObj = globalObj;
  this->serialMidi = serialMidi;
  this->saveFile = saveFile;
  this->adc = adc;
  this->cv2type[0] = 0;
  this->cv2type[1] = 0;
  this->cv2type[2] = 0;
  this->cv2type[3] = 0;
  this->cv2speed[0] = 1;
  this->cv2speed[1] = 1;
  this->cv2speed[2] = 1;
  this->cv2speed[3] = 1;
  this->cv2amplitude[0] = 0;
  this->cv2amplitude[1] = 0;
  this->cv2amplitude[2] = 0;
  this->cv2amplitude[3] = 0;
  this->lfoRheoSet[0] = 1;
  this->lfoRheoSet[1] = 1;
  this->lfoRheoSet[2] = 1;
  this->lfoRheoSet[3] = 1;
  this->lfoStartFrame[0] = 0;
  this->lfoStartFrame[1] = 0;
  this->lfoStartFrame[2] = 0;
  this->lfoStartFrame[3] = 0;
  this->sampleAndHoldSwitch[0] = 1;
  this->sampleAndHoldSwitch[1] = 1;
  this->sampleAndHoldSwitch[2] = 1;
  this->sampleAndHoldSwitch[3] = 1;

  Serial.println("initializing gate outputs");
  this->backplaneGPIO = backplaneGPIO;
  backplaneGPIO->begin(BACKPLANE_MAX7301_CS_PIN);
  backplaneGPIO->initPort(0, 8, OUTPUT); // gate out ch 1
  backplaneGPIO->initPort(1, 4, OUTPUT); // gate out ch 2
  backplaneGPIO->initPort(2, 9, OUTPUT); // gate out ch 3
  backplaneGPIO->initPort(3, 5, OUTPUT); // gate out ch 4

  backplaneGPIO->initPort(17, 6, OUTPUT); // clock output

  backplaneGPIO->initPort(4, 10, INPUT_PULLUP); // Gate in 1
  backplaneGPIO->initPort(5, 7, INPUT_PULLUP);  // Gate input 2
  backplaneGPIO->initPort(6, 11, INPUT_PULLUP); // Gate input 3
  backplaneGPIO->initPort(7, 12, INPUT_PULLUP); // Gate\ input 4
                                                /*
10  P14  10
11  P11   7
12  P15  11
13  P16  12
*/
  backplaneGPIO->initPort(8, 27, OUTPUT);       // latch reset

  backplaneGPIO->initPort(9, 18, OUTPUT);  // slew cap ctrl
  backplaneGPIO->initPort(10, 19, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(11, 20, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(12, 21, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(13, 22, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(14, 23, OUTPUT); // slew cap ctrl
  backplaneGPIO->initPort(15, 3, OUTPUT);  // slew cap ctrl
  backplaneGPIO->initPort(16, 24, OUTPUT); // slew cap ctrl

  backplaneGPIO->initPort(22, 14, INPUT_PULLUP); //Encoder Switch

  backplaneGPIO->initPort(18, 2, INPUT_PULLUP);

  backplaneGPIO->initPort(19, 1, INPUT_PULLUP);
  backplaneGPIO->initPort(20, 26, INPUT_PULLUP);
  backplaneGPIO->initPort(21, 13, INPUT_PULLUP);
  backplaneGPIO->initPort(23, 15, INPUT_PULLUP);
  backplaneGPIO->initPort(24, 16, INPUT_PULLUP);
  backplaneGPIO->initPort(25, 17, INPUT_PULLUP);
  backplaneGPIO->initPort(26, 25, INPUT_PULLUP);
  backplaneGPIO->initPort(27, 0, INPUT_PULLUP);

  backplaneGPIO->updateGpioPinModes(); // send GPIO pin modes to chip

  //backplaneGPIO->digitalWrite(0, 0);
  //backplaneGPIO->digitalWrite(1, 0);
  //backplaneGPIO->digitalWrite(2, 0);
  //backplaneGPIO->digitalWrite(3, 0);
  //backplaneGPIO->digitalWrite(8, 0);
  //backplaneGPIO->digitalWrite(4, 0);
  //backplaneGPIO->digitalWrite(5, 0);
  //backplaneGPIO->digitalWrite(6, 0);
  //backplaneGPIO->digitalWrite(7, 0);
  //backplaneGPIO->digitalWrite(8, 0);
  //backplaneGPIO->digitalWrite(9, 0);
  //backplaneGPIO->digitalWrite(10, 0);
  //backplaneGPIO->digitalWrite(11, 0);
  //backplaneGPIO->digitalWrite(12, 0);
  //backplaneGPIO->digitalWrite(13, 0);
  //backplaneGPIO->digitalWrite(18, 0);
  //backplaneGPIO->digitalWrite(14, 0);
  //backplaneGPIO->digitalWrite(15, 0);
  //backplaneGPIO->digitalWrite(16, 0);
  //backplaneGPIO->digitalWrite(17, 0);
  //backplaneGPIO->digitalWrite(18, 0);
  //backplaneGPIO->digitalWrite(19, 0);
  //backplaneGPIO->digitalWrite(20, 0);
  //backplaneGPIO->digitalWrite(21, 0);
  //backplaneGPIO->digitalWrite(22, 0);
  //backplaneGPIO->digitalWrite(23, 0);
  //backplaneGPIO->digitalWrite(28, 0);
  //backplaneGPIO->digitalWrite(24, 0);
  //backplaneGPIO->digitalWrite(25, 0);
  //backplaneGPIO->digitalWrite(26, 0);
  //backplaneGPIO->digitalWrite(27, 0);

  Serial.println("Initializing DAC");
  // PUT STUFF LIKE THIS INSIDE CONSTRUCTORS
  ad5676.begin(AD5676_CSPIN);
  delay(10);

  mcp4352_1.initialize(19);
  delay(10);

  mcp4352_2.initialize(18);
  delay(10);

  for (int i = 0; i < 4; i++)
  {
    // Serial.println("Resistance 1: " + String(i) + "- " + String(mcp4352_1.readResistance(i))); delay(1);
    // Serial.println("Resistance 2: " + String(i) + "- " + String(mcp4352_2.readResistance(i))); delay(1);
    delay(1);
    mcp4352_1.setResistance(i, 1);
    delay(1);
    mcp4352_2.setResistance(i, 1);
    delay(1);
    delay(1);
    // Serial.println("Resistance 1: " + String(i) + "- " + String(mcp4352_1.readResistance(i))); delay(1);
    // Serial.println("Resistance 2: " + String(i) + "- " + String(mcp4352_2.readResistance(i))); delay(1);
    delay(1);
    mcp4352_1.setResistance(i, 255);
    delay(1);
    mcp4352_2.setResistance(i, 255);
    delay(1);
    delay(1);
    // Serial.println("Resistance 1: " + String(i) + "- " + String(mcp4352_1.readResistance(i))); delay(1);
    // Serial.println("Resistance 2: " + String(i) + "- " + String(mcp4352_2.readResistance(i))); delay(1);
  }

  for (int i = 0; i < 4; i++)
  {
    backplaneGPIO->digitalWrite(outputMap(i, SLEWSWITCHCV), HIGH); // shut off swich with cap to ground, disable slew
    backplaneGPIO->digitalWrite(outputMap(i, SLEWSWITCHCC), HIGH); // shut off swich with cap to ground, disable slew
  }

  Serial.println("Setting up debug pin");
  pinMode(PIN_EXT_TX, OUTPUT);
  pinMode(4, OUTPUT);

  Serial.println("Output Controller Initialization Complete");

  backplaneGPIO->update();
  //for (int i=0; i<28; i++){
  //  Serial.println("Reading port: " + String(i) + "\t" + String(backplaneGPIO->readAddress(i)));
  //}
  this->setClockOutput(HIGH);
}

void OutputController::sequenceArrayResetSequence(uint8_t channel)
{
  sequenceArray[channel].clockReset(true);
};

void OutputController::flashMemoryStaggeredLoadLoop()
{
  saveFile->staggeredLoadLoop();
};

void OutputController::flashMemoryLoadPattern(uint8_t pattern, uint8_t channelSelector)
{
  saveFile->loadPattern(pattern, channelSelector);
};

void OutputController::sysexMessageHandler(char *sysex_message)
{
  saveFile->handleSysexData(sysex_message);
}

int OutputController::flashMemoryControl(int value)
{
  return saveFile->flashMemoryControl(value);
}

void OutputController::setDacVoltage(uint8_t dac, uint16_t output)
{
  ad5676.setVoltage(dac, output);
  ad5676.setVoltage(dac, output);
};

void OutputController::dacTestLoop()
{
  int voltage; // = 65535*(sin(millis()/10.0)+1)/2;
  int32_t sinVal = sin(millis() / 10.0) * 16384.0;
  bool connected = 0;
  if (sin(millis()) > 0)
  {
    voltage = 65535;
  }
  else
  {
    voltage = 0;
  }

  switch (stepMode)
  {

  case STATE_CALIB_INPUT0_OFFSET:
  case STATE_CALIB_INPUT1_OFFSET:
  case STATE_CALIB_INPUT2_OFFSET:
  case STATE_CALIB_INPUT3_OFFSET:
    for (int i = 0; i < 8; i++)
    {
      ad5676.setVoltage(dacMap[i], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[i]], globalObj->dacCalibrationPos[dacMap[i]]));
      ad5676.setVoltage(dacMap[i], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[i]], globalObj->dacCalibrationPos[dacMap[i]]));
    }
    break;
  case STATE_CALIB_INPUT0_LOW:
  case STATE_CALIB_INPUT1_LOW:
  case STATE_CALIB_INPUT2_LOW:
  case STATE_CALIB_INPUT3_LOW:
    for (int i = 0; i < 8; i++)
    {
      ad5676.setVoltage(dacMap[i], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[i]], globalObj->dacCalibrationPos[dacMap[i]]));
      ad5676.setVoltage(dacMap[i], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[i]], globalObj->dacCalibrationPos[dacMap[i]]));
      // ad5676.setVoltage(dacMap[i], map(16384, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[i]], globalObj->dacCalibrationPos[dacMap[i]] ) );
      // ad5676.setVoltage(dacMap[i], map(16384, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[i]], globalObj->dacCalibrationPos[dacMap[i]] ) );
    }
    break;
  case STATE_CALIB_INPUT1_HIGH:
  case STATE_CALIB_INPUT0_HIGH:
  case STATE_CALIB_INPUT2_HIGH:
  case STATE_CALIB_INPUT3_HIGH:
    for (int i = 0; i < 8; i++)
    {
      ad5676.setVoltage(dacMap[i], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[i]], globalObj->dacCalibrationPos[dacMap[i]]));
      ad5676.setVoltage(dacMap[i], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[i]], globalObj->dacCalibrationPos[dacMap[i]]));
    }
    break;

  case STATE_TEST_MIDI:

    //    Serial.println("Sending Midi Test Notes");
    //    delay(1000);
    //  for(int i=1; i<10; i++){
    //serialMidi->sendNoteOn(i, 127, 1);
    //    serialMidi->sendNoteOff(i, 0,  1);
    //delay(10);
    //}
    //      serialMidi->sendNoteOn(4, 64, 3);
    //      serialMidi->sendNoteOff(4, 0, 3);
    //  serialMidi->read();
    break;

  case STATE_TEST_GATES:
    //Serial.println("lets do this!");

    //activate the one gate that is active for this loop

    this->inputRead();

    if (globalObj->testTimer > 100)
    {
      connected = false;
      for (int i = 0; i < 4; i++)
      {
        if (globalObj->gateInputRaw[i] == true)
        {
          globalObj->gateTestArray[i] = globalObj->activeGate;
          Serial.println("Setting Gate out " + String(globalObj->activeGate) + " to " + String(i));
          connected = true;
        }
      }
      if (!connected)
      {
        globalObj->gateTestArray[globalObj->activeGate] = 255;
        Serial.println("Setting Gate out " + String(globalObj->activeGate) + " to " + String(255));
      }

      globalObj->activeGate = (globalObj->activeGate + 1) % 4;
      globalObj->testTimer = 0;
    }

    for (int n = 0; n < 4; n++)
    {
      if (n == globalObj->activeGate)
      {
        backplaneGPIO->digitalWrite(n, HIGH);
      }
      else
      {
        backplaneGPIO->digitalWrite(n, LOW);
      }
    }
    break;

  case STATE_TEST_RHEOSTAT:

    if (globalObj->testTimer > 150 && globalObj->activeGate != 1)
    {
      globalObj->activeGate = 1;
      for (int n = 0; n < 4; n++)
      {
        backplaneGPIO->digitalWrite(outputMap(n, SLEWSWITCHCC), LOW); // enable slew
        backplaneGPIO->digitalWrite(outputMap(n, SLEWSWITCHCV), LOW); // turn on switch with cap to ground, enable slew

        if (outputMap(n, RHEOCHANNELCV) == 0)
        {
          mcp4352_1.setResistance(outputMap(n, CVRHEO), globalObj->rheoTestLevel); // set digipot to 0
        }
        else
        {
          mcp4352_2.setResistance(outputMap(n, CVRHEO), globalObj->rheoTestLevel); // set digipot to 0
        }
        if (outputMap(n, RHEOCHANNELCC) == 0)
        {
          mcp4352_1.setResistance(outputMap(n, CCRHEO), globalObj->rheoTestLevel); // set digipot to 0
        }
        else
        {
          mcp4352_2.setResistance(outputMap(n, CCRHEO), globalObj->rheoTestLevel); // set digipot to 0
        }
      }

      for (int i = 0; i < 8; i++)
      {
        ad5676.setVoltage(dacMap[i], globalObj->dacCalibrationPos[dacMap[i]]);
        ad5676.setVoltage(dacMap[i], globalObj->dacCalibrationPos[dacMap[i]]);
      }
    }
    else if (globalObj->testTimer > 300)
    {
      globalObj->testTimer = 0;
      globalObj->activeGate = 0;
      globalObj->rheoTestLevel = (globalObj->rheoTestLevel + 16) % 128;
      for (int n = 0; n < 4; n++)
      {
        backplaneGPIO->digitalWrite(outputMap(n, SLEWSWITCHCC), HIGH); // disable slew
        backplaneGPIO->digitalWrite(outputMap(n, SLEWSWITCHCV), HIGH); // turn on switch with cap to ground, enable slew

        if (outputMap(n, RHEOCHANNELCV) == 0)
        {
          mcp4352_1.setResistance(outputMap(n, CVRHEO), 0); // set digipot to 0
        }
        else
        {
          mcp4352_2.setResistance(outputMap(n, CVRHEO), 0); // set digipot to 0
        }
        if (outputMap(n, RHEOCHANNELCC) == 0)
        {
          mcp4352_1.setResistance(outputMap(n, CCRHEO), 0); // set digipot to 0
        }
        else
        {
          mcp4352_2.setResistance(outputMap(n, CCRHEO), 0); // set digipot to 0
        }
      }

      for (int i = 0; i < 8; i++)
      {
        ad5676.setVoltage(dacMap[i], globalObj->dacCalibrationNeg[dacMap[i]]);
        ad5676.setVoltage(dacMap[i], globalObj->dacCalibrationNeg[dacMap[i]]);
      }
    }

    // for (int i=0; i<8; i++){
    //   ad5676.setVoltage(dacMap[i], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );
    //   ad5676.setVoltage(dacMap[i], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );
    // }
    break;
    /*
    case STATE_CALIB_OUTPUT0_LOW:
      ad5676.setVoltage(dacMap[0], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );
      ad5676.setVoltage(dacMap[0], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );
    break;

    case STATE_CALIB_OUTPUT0_HIGH:
      ad5676.setVoltage(dacMap[0], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );
      ad5676.setVoltage(dacMap[0], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );
    break;

    case STATE_CALIB_OUTPUT0_TEST:
  //    ad5676.setVoltage(dacMap[0], map(sinVal, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );
  //    ad5676.setVoltage(dacMap[0], map(sinVal, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );
      ad5676.setVoltage(dacMap[0], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );
      ad5676.setVoltage(dacMap[0], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[0]], globalObj->dacCalibrationPos[dacMap[0]] ) );

    break;
    case STATE_CALIB_OUTPUT1_LOW:
      ad5676.setVoltage(dacMap[1], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[1]], globalObj->dacCalibrationPos[dacMap[1]] ) );
      ad5676.setVoltage(dacMap[1], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[1]], globalObj->dacCalibrationPos[dacMap[1]] ) );
    break;
    case STATE_CALIB_OUTPUT1_HIGH:
      ad5676.setVoltage(dacMap[1], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[1]], globalObj->dacCalibrationPos[dacMap[1]] ) );
      ad5676.setVoltage(dacMap[1], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[1]], globalObj->dacCalibrationPos[dacMap[1]] ) );
    break;
    case STATE_CALIB_OUTPUT1_TEST:
      ad5676.setVoltage(dacMap[1], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[1]], globalObj->dacCalibrationPos[dacMap[1]] ) );
      ad5676.setVoltage(dacMap[1], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[1]], globalObj->dacCalibrationPos[dacMap[1]] ) );
    break;
    case STATE_CALIB_OUTPUT2_LOW:
      ad5676.setVoltage(dacMap[2], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[2]], globalObj->dacCalibrationPos[dacMap[2]] ) );
      ad5676.setVoltage(dacMap[2], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[2]], globalObj->dacCalibrationPos[dacMap[2]] ) );
    break;
    case STATE_CALIB_OUTPUT2_HIGH:
      ad5676.setVoltage(dacMap[2], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[2]], globalObj->dacCalibrationPos[dacMap[2]] ) );
      ad5676.setVoltage(dacMap[2], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[2]], globalObj->dacCalibrationPos[dacMap[2]] ) );
    break;
    case STATE_CALIB_OUTPUT2_TEST:
      ad5676.setVoltage(dacMap[2], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[2]], globalObj->dacCalibrationPos[dacMap[2]] ) );
      ad5676.setVoltage(dacMap[2], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[2]], globalObj->dacCalibrationPos[dacMap[2]] ) );
    break;
    case STATE_CALIB_OUTPUT3_LOW:
      ad5676.setVoltage(dacMap[3], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[3]], globalObj->dacCalibrationPos[dacMap[3]] ) );
      ad5676.setVoltage(dacMap[3], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[3]], globalObj->dacCalibrationPos[dacMap[3]] ) );
    break;
    case STATE_CALIB_OUTPUT3_HIGH:
      ad5676.setVoltage(dacMap[3], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[3]], globalObj->dacCalibrationPos[dacMap[3]] ) );
      ad5676.setVoltage(dacMap[3], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[3]], globalObj->dacCalibrationPos[dacMap[3]] ) );
    break;
    case STATE_CALIB_OUTPUT3_TEST:
      ad5676.setVoltage(dacMap[3], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[3]], globalObj->dacCalibrationPos[dacMap[3]] ) );
      ad5676.setVoltage(dacMap[3], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[3]], globalObj->dacCalibrationPos[dacMap[3]] ) );
    break;
    case STATE_CALIB_OUTPUT4_LOW:
      ad5676.setVoltage(dacMap[4], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[4]], globalObj->dacCalibrationPos[dacMap[4]] ) );
      ad5676.setVoltage(dacMap[4], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[4]], globalObj->dacCalibrationPos[dacMap[4]] ) );
    break;
    case STATE_CALIB_OUTPUT4_HIGH:
      ad5676.setVoltage(dacMap[4], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[4]], globalObj->dacCalibrationPos[dacMap[4]] ) );
      ad5676.setVoltage(dacMap[4], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[4]], globalObj->dacCalibrationPos[dacMap[4]] ) );
    break;
    case STATE_CALIB_OUTPUT4_TEST:
      ad5676.setVoltage(dacMap[4], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[4]], globalObj->dacCalibrationPos[dacMap[4]] ) );
      ad5676.setVoltage(dacMap[4], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[4]], globalObj->dacCalibrationPos[dacMap[4]] ) );
    break;
    case STATE_CALIB_OUTPUT5_LOW:
      ad5676.setVoltage(dacMap[5], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[5]], globalObj->dacCalibrationPos[dacMap[5]] ) );
      ad5676.setVoltage(dacMap[5], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[5]], globalObj->dacCalibrationPos[dacMap[5]] ) );
    break;
    case STATE_CALIB_OUTPUT5_HIGH:
      ad5676.setVoltage(dacMap[5], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[5]], globalObj->dacCalibrationPos[dacMap[5]] ) );
      ad5676.setVoltage(dacMap[5], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[5]], globalObj->dacCalibrationPos[dacMap[5]] ) );
    break;
    case STATE_CALIB_OUTPUT5_TEST:
      ad5676.setVoltage(dacMap[5], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[5]], globalObj->dacCalibrationPos[dacMap[5]] ) );
      ad5676.setVoltage(dacMap[5], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[5]], globalObj->dacCalibrationPos[dacMap[5]] ) );
    break;
    case STATE_CALIB_OUTPUT6_LOW:
      ad5676.setVoltage(dacMap[6], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[6]], globalObj->dacCalibrationPos[dacMap[6]] ) );
      ad5676.setVoltage(dacMap[6], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[6]], globalObj->dacCalibrationPos[dacMap[6]] ) );
    break;
    case STATE_CALIB_OUTPUT6_HIGH:
      ad5676.setVoltage(dacMap[6], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[6]], globalObj->dacCalibrationPos[dacMap[6]] ) );
      ad5676.setVoltage(dacMap[6], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[6]], globalObj->dacCalibrationPos[dacMap[6]] ) );
    break;
    case STATE_CALIB_OUTPUT6_TEST:
      ad5676.setVoltage(dacMap[6], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[6]], globalObj->dacCalibrationPos[dacMap[6]] ) );
      ad5676.setVoltage(dacMap[6], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[6]], globalObj->dacCalibrationPos[dacMap[6]] ) );
    break;
    case STATE_CALIB_OUTPUT7_LOW:
      ad5676.setVoltage(dacMap[7], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[7]], globalObj->dacCalibrationPos[dacMap[7]] ) );
      ad5676.setVoltage(dacMap[7], map(-8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[7]], globalObj->dacCalibrationPos[dacMap[7]] ) );
    break;
    case STATE_CALIB_OUTPUT7_HIGH:
      ad5676.setVoltage(dacMap[7], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[7]], globalObj->dacCalibrationPos[dacMap[7]] ) );
      ad5676.setVoltage(dacMap[7], map(8192, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[7]], globalObj->dacCalibrationPos[dacMap[7]] ) );
    break;
    case STATE_CALIB_OUTPUT7_TEST:
      ad5676.setVoltage(dacMap[7], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[7]], globalObj->dacCalibrationPos[dacMap[7]] ) );
      ad5676.setVoltage(dacMap[7], map(0, -16384, 16384, globalObj->dacCalibrationNeg[dacMap[7]], globalObj->dacCalibrationPos[dacMap[7]] ) );
    break;
    */
  }

  //Serial.println("setting voltage all dacs to: " + String(voltage) + " -- starttime:" + String(startTime));

  for (int i = 0; i < 8; i++)
  {

    //    ad5676.setVoltage(i, voltage );
    //    delayMicroseconds(1);

    //    ad5676.setVoltage(i, voltage );
    //    delayMicroseconds(1);
    //ad5676.setVoltage(i, 65535sin(millis()/2) );
  }
}

void OutputController::inputLoopTest()
{
  //  backplaneGPIO->update();
  bool trig = false;
  for (int i = 0; i < 28; i++)
  {
    if (backplaneGPIO->fell(i))
    {
      //  Serial.println("*+_*+_*+*+_*+_*+*+*+_*  Backplane input fell: " + String(i));
      trig = true;
    }
  }

  if (trig)
  {
    Serial.println("CV1: " + String(analogRead(22)) + "\tCV2: " + String(analogRead(21)) + "\tCV3: " + String(analogRead(20)) + "\tCV4: " + String(analogRead(23)));
  }
  //backplaneGPIO->digitalWrite(8, 1);
  delay(1);
  //backplaneGPIO->digitalWrite(8, 0);
}

void OutputController::inputRead()
{
  backplaneGPIO->update();

  for (int i = 0; i < 28; i++)
  {
    //  if(backplaneGPIO->fell(i)) Serial.println("Backplane " + String(i));
  }
  globalObj->cvInputRaw[0] = adc->analogRead(A3, ADC_1) / 2;
  globalObj->cvInputRaw[1] = adc->analogRead(A12, ADC_1) / 2;
  globalObj->cvInputRaw[2] = adc->analogRead(A13, ADC_1) / 2;
  globalObj->cvInputRaw[3] = adc->analogRead(A10, ADC_1) / 2;

  for (int ch = 0; ch < 4; ch++)
  {
    //globalObj->cvInputMapped[ch] = map(globalObj->cvInputRaw[ch], globalObj->adcCalibrationNeg[ch], globalObj->adcCalibrationPos[ch], -60, 60) +1;
    if (globalObj->cvInputRaw[ch] < globalObj->adcCalibrationOffset[ch])
    {
      globalObj->cvInputMapped1024[ch] = map(globalObj->cvInputRaw[ch], globalObj->adcCalibrationPos[ch], globalObj->adcCalibrationOffset[ch], 1024, 0);
    }
    else
    {
      globalObj->cvInputMapped1024[ch] = map(globalObj->cvInputRaw[ch], globalObj->adcCalibrationOffset[ch], globalObj->adcCalibrationNeg[ch], 0, -1024);
    }
    globalObj->cvInputMapped[ch] = map(globalObj->cvInputMapped1024[ch], -1024, 1024, -64, 64);
  }

  for (int i = 4; i < 13; i++)
  {
    switch (i)
    {
    case 4:
    case 6:
    case 8:
    case 10:
      globalObj->cvInputMapped[i] = globalObj->cvInputRaw[i] - 24;
      break;
    case 5:
    case 7:
    case 9:
    case 11:
      globalObj->cvInputMapped[i] = map(globalObj->cvInputRaw[i], -16384, 16384, -64, 64);
      break;
    }
  }

  globalObj->gateInputRaw[0] = !backplaneGPIO->pressed(4);
  globalObj->gateInputRaw[1] = !backplaneGPIO->pressed(5);
  globalObj->gateInputRaw[2] = !backplaneGPIO->pressed(6);
  globalObj->gateInputRaw[3] = !backplaneGPIO->pressed(7);

  globalObj->gateInputRose[0] = backplaneGPIO->rose(4);
  globalObj->gateInputRose[1] = backplaneGPIO->rose(5);
  globalObj->gateInputRose[2] = backplaneGPIO->rose(6);
  globalObj->gateInputRose[3] = backplaneGPIO->rose(7);

  for (int i = 0; i < 4; i++)
  { // might be able to do this with -> rose too... not sure. need to test.
    if (backplaneGPIO->cacheCheck(i) && (globalObj->gateInputRaw[i + 4] != backplaneGPIO->cacheCheck(i)))
    {
      globalObj->gateInputRose[i + 4] = true;
    }
    else
    {
      globalObj->gateInputRose[i + 4] = false;
    }
    globalObj->gateInputRaw[i + 4] = backplaneGPIO->cacheCheck(i);
  }

  //reset SR Latch
  backplaneGPIO->digitalWrite(8, 1);
  backplaneGPIO->digitalWrite(8, 0);
}

int16_t OutputController::cvInputCheck(uint8_t mapValue)
{
  if (mapValue == 0)
  {
    return 0;
  }
  else
  {
    return globalObj->cvInputMapped[mapValue - 1];
  };
}

bool OutputController::gpioCheck(int8_t mapValue)
{
  // for gpiocheck - needs rename - values that are less than
  // values that are 0 are unmapped
  // values that are negative are percentage chance
  // values between 1-4 are Gate inputs
  // values 5, 6, 7, 8 are internally mapped gates from each channel
  switch (mapValue)
  {
  case 0: // no mapping
    return 0;
    break;
  case 1:
    return globalObj->gateInputRaw[0];
    break; //gate input 1
  case 2:
    return globalObj->gateInputRaw[1];
    break; //gate input 2
  case 3:
    return globalObj->gateInputRaw[2];
    break; //gate input 3
  case 4:
    return globalObj->gateInputRaw[3];
    break; //gate input 4

  case 5:
    return globalObj->gateInputRaw[4];
    break; //gate out ch1
  case 6:
    return globalObj->gateInputRaw[5];
    break; //gate out ch2
  case 7:
    return globalObj->gateInputRaw[6];
    break; //gate out ch3
  case 8:
    return globalObj->gateInputRaw[7];
    break; //gate out ch4

  case -1:
    return (random(100) > 90);
    break;
  case -2:
    return (random(100) > 80);
    break;
  case -3:
    return (random(100) > 70);
    break;
  case -4:
    return (random(100) > 60);
    break;
  case -5:
    return (random(100) > 50);
    break;
  case -6:
    return (random(100) > 40);
    break;
  case -7:
    return (random(100) > 30);
    break;
  case -8:
    return (random(100) > 20);
    break;
  case -9:
    return (random(100) > 10);
    break;
  }
};

void OutputController::noteOn(uint8_t channel, uint8_t stepNum, uint16_t note, uint8_t velocity, uint8_t velocityType, uint8_t cv2speedSetting, uint8_t cv2offsetSetting, uint8_t glide, bool gate, bool tieFlag, uint8_t quantizeScale, uint16_t quantizeMode, uint8_t quantizeKey, bool cvMute, uint32_t startFrame, bool notFirstArp)
{
  // proto 6 calibration numbers: 0v: 22180   5v: 43340
  // Serial.println("    OutputController -- on ch:" + String(channel) + " nt: " + String(note));
  /*  proto 8 basic calibration
10v - 64240
5v - 48570
0v - 32896
-5v - 17210
-10v  1540
*/
  // update modulation destinations
  globalObj->cvInputRaw[4 + 2 * channel] = note;
  //  globalObj->cvInputMapped[4+2*channel] = note;

  //Serial.println("begin note on ch: " + String(channel) + "\tnote: " + String(note) + "\tvel: "+ String(velocity) + "\tglide: " + String(glide) + "\tgate: " + String(gate));
  if (gate)
  {
    if (backplaneGPIO->cacheCheck(channel) == 1 && tieFlag == 0)
    {
      backplaneGPIO->digitalWrite(channel, LOW); // close gate before re opening
      //  delay(1);
      //Serial.println("setting gate low because it was still on.");
    }
  }

  if (glide == 0 && tieFlag)
  {
    glide = 3; // this is where tempo dependent glide needs to be set
  }

  if (glide > 0)
  {
    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCV), LOW); // turn on switch with cap to ground, enable slew
    if (outputMap(channel, RHEOCHANNELCV) == 0)
    {
      mcp4352_1.setResistance(outputMap(channel, CVRHEO), map(glide, 0, 127, 0, 255)); // set digipot to correct resistance, set slew rate
    }
    else
    {
      mcp4352_2.setResistance(outputMap(channel, CVRHEO), map(glide, 0, 127, 0, 255)); // set digipot to correct resistance, set slew rate
    }
    //  Serial.println("glide  ch: " + String(channel) + "\ton dacCh: " + String(dacCvMap[channel]) + "\tCVrheo: " + String(outputMap(channel, CVRHEO)) + "\ton mcp4352 " +  String(outputMap(channel, RHEOCHANNELCV)) + "\t with slew switch: " + String(outputMap(channel, SLEWSWITCHCV)) + "\tslewSetting: " + String(map(glide, 0,127,0,255)) );
  }
  else
  {
    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCV), HIGH); // shut off swich with cap to ground, disable slew
    if (outputMap(channel, RHEOCHANNELCV) == 0)
    {
      mcp4352_1.setResistance(outputMap(channel, CVRHEO), 0); // set digipot to 0
    }
    else
    {
      mcp4352_2.setResistance(outputMap(channel, CVRHEO), 0); // set digipot to 0
    }
  }
  if (velocityType != 0)
  {
    cv2currentStep[channel] = stepNum;
    cv2type[channel] = velocityType;
    cv2amplitude[channel] = velocity;
    cv2speed[channel] = cv2speedSetting;
    cv2offset[channel] = cv2offsetSetting;

    lfoStartFrame[channel] = startFrame;
  }

  //Serial.println("NO glide  ch: " + String(channel) + "\ton dacCh: " + String(dacCvMap[channel]) + "\tCVrheo: " + String(outputMap(channel, CVRHEO)) + "\ton mcp4352 " +  String(outputMap(channel, RHEOCHANNELCV)) + "\t with slew switch: " + String(outputMap(channel, SLEWSWITCHCV)) + "\tslewSetting: " + String(map(glide, 0,127,0,255)) );

  int offset = 0;
  //offset = globalObj->cvInputMapped[channel];
  //delayMicroseconds(5);
  //  ad5676.setVoltage(dacCvMap[channel], map( (note+offset), 0,120,calibLow(channel, dacCvMap[channel], 0), calibHigh(channel, dacCvMap[channel],0)) );    // set CV voltage
  if (!cvMute)
  {
    ad5676.setVoltage(dacCvMap[channel], scale_degree_to_quantized_dac_output(channel, note, globalObj->outputNegOffset[channel], 1)); // set CV voltage
                                                                                                                                       //delayMicroseconds(5);
    //ad5676.setVoltage(dacCvMap[channel], map( (note+offset), 0,120,calibLow(channel, dacCvMap[channel], 0), calibHigh(channel, dacCvMap[channel], 0)));    // set CV voltage
    ad5676.setVoltage(dacCvMap[channel], scale_degree_to_quantized_dac_output(channel, note, globalObj->outputNegOffset[channel], 1)); // set CV voltage
                                                                                                                                       //  delayMicroseconds(5);
    //Serial.println("Ch " + String(channel) + "\t offset:" + String(offset) + "\traw: " + String(globalObj->cvInputRaw[channel]));
    serialMidi->sendNoteOn(note, velocity, globalObj->midiChannel[channel]); // send midi note out
    usbMIDI.sendNoteOn(note, velocity, globalObj->midiChannel[channel]);     // send midi note out
  }

  if (gate)
  {
    backplaneGPIO->digitalWrite(channel, HIGH); // open gate voltage
  }
};

uint16_t OutputController::scale_degree_to_quantized_dac_output(uint8_t channel, uint16_t note, uint8_t negOffset, bool pitchValue)
{
  uint32_t cents = 0;
  uint16_t quantized_dac_code = 0;
  uint16_t calibrationLow = 0;
  uint16_t calibrationHigh = 0;

  if (pitchValue)
  {
    calibrationLow = calibZero(channel, dacCvMap[channel], negOffset);
    calibrationHigh = calibHigh(channel, dacCvMap[channel], negOffset);
  }
  else
  {
    calibrationLow = -16384;
    calibrationHigh = 16384;
  }
  if (sequenceArray[channel].quantize_is_edo_scale)
  {
    quantized_dac_code = globalObj->quantize_edo_scale_degree_to_key(note, sequenceArray[channel].quantizeKey, sequenceArray[channel].quantizeMode, sequenceArray[channel].quantizeScale, 0);
    // quantized_dac_code = globalObj->quantize_edo_scale_degree_to_key(note, sequenceArray[channel].quantizeKey, 0, sequenceArray[channel].quantizeScale, 0);
    // Serial.printf("%04d ", quantized_dac_code);
    quantized_dac_code = globalObj->convert_edo_scale_degree_to_dac_code(quantized_dac_code, sequenceArray[channel].quantizeScale);
    // Serial.printf("%04d ", quantized_dac_code);
    if (pitchValue)
    {
      quantized_dac_code = map(quantized_dac_code, 0, 65536, calibrationLow, calibrationHigh);
    }
    else
    {
      quantized_dac_code = map(quantized_dac_code, sequenceArray[channel].quantizeScale * -10, sequenceArray[channel].quantizeScale * 10, calibrationLow, calibrationHigh);
    }
    // Serial.printf("scale: %04d key: %03d mode: %04d note: %03d \tdac_code: %04d\n", sequenceArray[channel].quantizeScale, sequenceArray[channel].quantizeKey, sequenceArray[channel].quantizeMode, note, quantized_dac_code);
  }
  else
  {
    switch (sequenceArray[channel].quantizeScale)
    {
    case SEMITONE:
      quantized_dac_code = globalObj->quantize_semitone_pitch(note, sequenceArray[channel].quantizeKey, sequenceArray[channel].quantizeMode, 0);
      if (pitchValue)
      {
        quantized_dac_code = map(quantized_dac_code, 0, 120, calibrationLow, calibrationHigh);
      }
      else
      {
        quantized_dac_code = map(quantized_dac_code, -120, 120, calibrationLow, calibrationHigh);
      }
      break;
    case PYTHAGOREAN:
      quantized_dac_code = globalObj->quantize_semitone_pitch(note, sequenceArray[channel].quantizeKey, sequenceArray[channel].quantizeMode, 0);
      cents = pythagorean10thCent[quantized_dac_code % pythagoreanNoteCount] + 12000 * quantized_dac_code / pythagoreanNoteCount;
      quantized_dac_code = map(cents, 0, 120000, calibrationLow, calibrationHigh);
      break;
    case COLUNDI:
      quantized_dac_code = map(colundiArrayX100[note], 0, 1012, calibrationLow, calibrationHigh);
      break;
    case TET_17:
      quantized_dac_code = globalObj->quantize_semitone_pitch(note, sequenceArray[channel].quantizeKey, sequenceArray[channel].quantizeMode, 0);
      quantized_dac_code = map(quantized_dac_code, 0, 190, calibrationLow, calibrationHigh);
      break;
    }
  }

  return quantized_dac_code;
}

uint16_t OutputController::voltageOffset(uint8_t volts, uint8_t mapAddress)
{
  uint16_t twentyVolts = globalObj->dacCalibrationPos[mapAddress] - globalObj->dacCalibrationNeg[mapAddress];
  return volts * twentyVolts / 20;
}

uint16_t OutputController::calibMidscale(uint8_t mapAddress)
{
  return (globalObj->dacCalibrationNeg[mapAddress] + globalObj->dacCalibrationPos[mapAddress]) / 2;
};

uint16_t OutputController::calibZero(uint8_t channel, uint8_t mapAddress, uint8_t negOffset)
{
  return calibMidscale(mapAddress) - voltageOffset(negOffset, mapAddress);
}

uint16_t OutputController::calibHigh(uint8_t channel, uint8_t mapAddress, uint8_t negOffset)
{
  return globalObj->dacCalibrationPos[mapAddress] - voltageOffset(negOffset, mapAddress);
}

void OutputController::clearVelocityOutput(uint8_t channel)
{
  int16_t voltageLevel = 0;

  for (int i = 0; i < SEQUENCECOUNT; i++)
  {
    this->cv2type[i] = 0;
    this->cv2speed[i] = 1;
    this->cv2amplitude[i] = 0;
    this->lfoRheoSet[i] = 1;
    this->lfoStartFrame[i] = 0;
    this->sampleAndHoldSwitch[i] = 1;
  }

  ad5676.setVoltage(dacCcMap[channel], map(voltageLevel, -16384, 16384, globalObj->dacCalibrationNeg[dacCcMap[channel]], globalObj->dacCalibrationPos[dacCcMap[channel]])); // set CC voltage
  ad5676.setVoltage(dacCcMap[channel], map(voltageLevel, -16384, 16384, globalObj->dacCalibrationNeg[dacCcMap[channel]], globalObj->dacCalibrationPos[dacCcMap[channel]])); // set CC voltage
};

void OutputController::cv2settingsChange(uint8_t channel, uint8_t stepNum, uint8_t velocity, uint8_t velocityType, uint8_t cv2speedSetting, uint8_t cv2offsetSetting)
{
  if (this->cv2currentStep[channel] == stepNum)
  {
    if (velocityType != 0)
    {
      this->cv2type[channel] = velocityType;
      this->cv2amplitude[channel] = velocity;
      this->cv2speed[channel] = cv2speedSetting;
      this->cv2offset[channel] = cv2offsetSetting;
    }
  }
}

void OutputController::cv2update(uint8_t channel, uint32_t currentFrame, uint32_t framesPerSequence, uint32_t stepLength, bool mute)
{
  if (mute)
  {
    return;
  }
  uint8_t slewLevel = 0;
  int16_t voltageLevel = 0;
  uint32_t offset = 0;
  uint32_t segmentLength = 0;
  uint32_t clockCycles = ARM_DWT_CYCCNT;
  uint32_t lfoTime;

  if (lfoStartFrame[channel] < currentFrame)
  {
    lfoTime = positive_modulo(currentFrame - lfoStartFrame[channel], framesPerSequence) * cv2speed[channel] / 4;
  }
  else
  {
    lfoTime = positive_modulo(currentFrame + framesPerSequence - lfoStartFrame[channel], framesPerSequence) * cv2speed[channel] / 4;
  }
  bool slewOn = false;
  bool skipUpdate = false;
  ///stepLength *= 8;
  //Serial.println("beginning cv2update for channel " + String(channel) );

  if (debugFrameCount > stepLength / 2 && currentFrame < stepLength / 2)
  {
    debugFrameCount = currentFrame + stepLength / 2;
  }

  if (cv2type[channel] < 1)
  {
    return;
  }

  switch (cv2type[channel])
  {
  case LFO_OFF:
    slewLevel = 0;
    voltageLevel = 0;
    break;

  case LFO_TRIGGER:
    if (currentFrame < lfoStartFrame[channel] + stepLength / 16)
    {
      voltageLevel = 16384;
    }
    else
    {
      voltageLevel = 0;
    }
    break;

  case LFO_QUANTIZED:
    voltageLevel = this->scale_degree_to_quantized_dac_output(channel, cv2amplitude[channel], 0, 0);
    slewLevel = 0;
    break;

  case LFO_ENV_AR:
    offset = stepLength * cv2speed[channel] / 8; //center point between attack and release
    segmentLength = (stepLength * 16 / (cv2speed[channel]));

    if ((currentFrame < lfoStartFrame[channel]) || (currentFrame > lfoStartFrame[channel] + stepLength * cv2speed[channel] / 4))
    {
      voltageLevel = 0;
    }
    else if (currentFrame < lfoStartFrame[channel] + offset)
    {
      voltageLevel = cv2amplitude[channel] * (128 * ((currentFrame - lfoStartFrame[channel]) / 128) / ((stepLength * cv2speed[channel] / 8) / 128));
    }
    else
    {
      voltageLevel = cv2amplitude[channel] * (128 - 128 * ((currentFrame - lfoStartFrame[channel] - offset) / 128) / ((stepLength * cv2speed[channel] / 8) / 128));
    }

    slewOn = false;
    //Serial.println("VoltLev: " + String(voltageLevel) + "\tcurrentFrame: " + String(currentFrame) + "\tlfoStart: " + String(lfoStartFrame[channel]) + "\tstepLength: " + String(stepLength) +  "\tdiv: " + String(100*(currentFrame - lfoStartFrame[channel])/stepLength));

    break;

  case LFO_ENV_ASR:

    offset = stepLength * cv2speed[channel] / 12; //center point between attack and release
    segmentLength = (stepLength * 16 / (cv2speed[channel]));

    if ((currentFrame < lfoStartFrame[channel]) || (currentFrame > lfoStartFrame[channel] + stepLength * cv2speed[channel] / 4))
    {
      voltageLevel = 0;
    }
    else if (currentFrame < lfoStartFrame[channel] + offset)
    {
      voltageLevel = cv2amplitude[channel] * (128 * ((currentFrame - lfoStartFrame[channel]) / 128) / ((stepLength * cv2speed[channel] / 12) / 128));
    }
    else if (currentFrame < lfoStartFrame[channel] + 2 * offset)
    {
      voltageLevel = cv2amplitude[channel] * 128;
    }
    else
    {
      voltageLevel = cv2amplitude[channel] * (128 - 128 * ((currentFrame - lfoStartFrame[channel] - 2 * offset) / 128) / ((stepLength * cv2speed[channel] / 12) / 128));
    }

    slewOn = false;
    break;

  case LFO_ENV_DECAY:
  GOTODECAY:

    if ((currentFrame < lfoStartFrame[channel]) || (currentFrame > lfoStartFrame[channel] + stepLength * cv2speed[channel] / 4))
    {
      voltageLevel = 0;
    }
    else
    {
      voltageLevel = cv2amplitude[channel] * (128 - 128 * ((currentFrame - lfoStartFrame[channel]) / 128) / ((stepLength * cv2speed[channel] / 4) / 128));
    }
    slewOn = false;

    //Serial.println("VoltLev: " + String(voltageLevel) + "\tcurrentFrame: " + String(currentFrame) + "\tlfoStart: " + String(lfoStartFrame[channel]) + "\tdiv: " + String(100*(currentFrame - lfoStartFrame[channel])/stepLength) + "\tvoltCalc2: " + String((128-128*((int32_t)currentFrame - lfoStartFrame[channel])/(stepLength*cv2speed[channel]/4))) + "\tvc3: " + String(128*(currentFrame - lfoStartFrame[channel])));
    break;

  case LFO_ENV_ATTACK:
  GOTOATTACK:

    if ((currentFrame < lfoStartFrame[channel]) || (currentFrame > lfoStartFrame[channel] + stepLength * cv2speed[channel] / 4))
    {
      voltageLevel = 0;
    }
    else
    {
      voltageLevel = cv2amplitude[channel] * (128 * ((currentFrame - lfoStartFrame[channel]) / 128) / ((stepLength * cv2speed[channel] / 4) / 128));
    }

    slewLevel = 0;
    slewOn = false;
    break;
  case LFO_TRIANGLE:
    voltageLevel = 2 * abs(cv2amplitude[channel] * ((int)(lfoTime / (stepLength / 16)) % (256) - 128)) - (cv2amplitude[channel] * 128);
    slewOn = false;
    slewLevel = 0;
    break;

  case LFO_SAWUP:
    slewLevel = 0;
    voltageLevel = cv2amplitude[channel] * ((int)(lfoTime / (stepLength / 16)) % (256) - 128);
    slewOn = false;
    break;

  case LFO_SAWDN:
    slewLevel = 0;
    //      voltageLevel = cv2amplitude[channel]*128 -  (int)(lfoTime/(stepLength/16)) % (cv2amplitude[channel]*256);
    voltageLevel = -1 * cv2amplitude[channel] * ((int)(lfoTime / (stepLength / 16)) % (256) - 128);

    slewOn = false;
    break;

  case LFO_SAMPLEHOLD:
    slewLevel = 0;
    slewOn = false;

    if (sin((lfoTime * 3.1415926536) / (8 * stepLength)) > 0)
    {
      if (sampleAndHoldSwitch[channel] == 1)
      {
        voltageLevel = random(-cv2amplitude[channel] * 128, cv2amplitude[channel] * 128);
        sampleAndHoldSwitch[channel] = 0;
        skipUpdate = false;
      }
      else
      {
        skipUpdate = true;
      }
    }
    else
    {
      sampleAndHoldSwitch[channel] = 1;
      skipUpdate = true;
    }
    break;

  case LFO_VOLTAGE:
  GOTOVOLTAGE:
    slewLevel = 0;
    voltageLevel = cv2amplitude[channel] * 128;
    //voltageLevel = clockCycles/4096;
    slewOn = false;
    break;

  case LFO_SINE:
    slewLevel = 1;
    voltageLevel = (sin((lfoTime * 3.1415926536) / (8 * stepLength))) * cv2amplitude[channel] * 128;
    slewOn = true;
    break;

  case LFO_SQUARE: // SQUARE WAVE
    slewLevel = 0;
    if (sin((lfoTime * 3.1415926536) / (8 * stepLength)) > 0)
    {
      voltageLevel = cv2amplitude[channel] * 128;
    }
    else
    {
      voltageLevel = -cv2amplitude[channel] * 128;
    }
    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCC), HIGH); // shut off swich with cap to ground, disable slew
    break;

  case LFO_RNDSQUARE: //rounded square wave
    slewLevel = 40;
    if (sin((lfoTime * 3.1415926536) / (8 * stepLength)) > 0)
    {
      voltageLevel = cv2amplitude[channel] * 128;
    }
    else
    {
      voltageLevel = -cv2amplitude[channel] * 128;
    }
    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCC), LOW); // shut off swich with cap to ground, disable slew

    break;

  default:
    //      return;
    break;
  }

  if (slewOn)
  {
    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCC), LOW); // enable slew
  }
  else
  {
    backplaneGPIO->digitalWrite(outputMap(channel, SLEWSWITCHCC), HIGH); // disable slew
  }
  //cv2amplitude[channel]
  //lfoRheoSet[channel]
  // update modulation destinations

  //add offset amount:
  if (channel == 0 && currentFrame > debugFrameCount)
  {
    // Serial.print("q:" + String(globalObj->quantize_semitone_pitch(cv2amplitude[channel], sequenceArray[channel].quantizeKey, sequenceArray[channel].quantizeMode, 0)));
  }
  voltageLevel += cv2offset[channel] * 128;

  if (voltageLevel > 16384)
  {
    voltageLevel = 16384 - (voltageLevel - 16384);
  }
  else if (voltageLevel < -16384)
  {
    voltageLevel = -16384 + (abs(voltageLevel) - 16384);
  }

  if (channel == 0 && currentFrame > debugFrameCount)
  {
    debugFrameCount += (stepLength / 2);
    debugFrameCount = debugFrameCount % framesPerSequence;
    // Serial.println("\tcf: " + String(currentFrame) + "\tlfoTime: " + String(lfoTime) + "\tlfoStart: " + String(lfoStartFrame[channel]) + "\tfps: " + String(framesPerSequence) + "\ttype: " + String(cv2type[channel]) + "\tvl: " + String(voltageLevel) + "\tamp: " + String(cv2amplitude[channel]) );
  }

  if (!skipUpdate)
  {
    globalObj->cvInputRaw[4 + 2 * channel + 1] = voltageLevel;

    if (outputMap(channel, RHEOCHANNELCC) == 0)
    {
      mcp4352_1.setResistance(outputMap(channel, CCRHEO), slewLevel); // set digipot to 0
    }
    else
    {
      mcp4352_2.setResistance(outputMap(channel, CCRHEO), slewLevel); // set digipot to 0
    }
    lfoRheoSet[channel] = 0;
    ad5676.setVoltage(dacCcMap[channel], map(voltageLevel, -16384, 16384, globalObj->dacCalibrationNeg[dacCcMap[channel]], globalObj->dacCalibrationPos[dacCcMap[channel]]));
    ad5676.setVoltage(dacCcMap[channel], map(voltageLevel, -16384, 16384, globalObj->dacCalibrationNeg[dacCcMap[channel]], globalObj->dacCalibrationPos[dacCcMap[channel]])); // set CC voltage
  }

  //  Serial.println("Setting velocity-ch:" + String(channel) + "\tVL: " + String(voltageLevel) + "\trheo: " + String(slewLevel) + "\ttype: " + String(cv2type[channel]) + "\tstartTime: " + String(startTime) + "\tbeatLength:" + String(beatLength) + "\tamp: " + String(cv2amplitude[channel]) + "\tsinResult:" + String(sin((startTime*3.14159)/(beatLength) )) + "\tdivide: " + String(startTime/beatLength) +"\tendVolt: " + String(map(voltageLevel, -127,127,0, 65535 )));
}

uint8_t OutputController::analogInputTranspose(uint8_t note)
{
  uint16_t input = analogRead(22);
  return (note + map(input, 0, 1023, 0, 127));
}

uint8_t OutputController::outputMap(uint8_t channel, uint8_t mapType)
{
  switch (mapType)
  {
  case CVMAP:
    return dacCvMap[channel];
    break;

  case CCMAP:
    return dacCcMap[channel];
    break;

  case SLEWSWITCHCV:
    return slewSwitchMap[dacCvMap[channel]];
    break;

  case SLEWSWITCHCC:
    return slewSwitchMap[dacCcMap[channel]];
    break;

  case RHEOCHANNELCV:
    return rheoMap[dacCvMap[channel]] >> 4;
    break;

  case RHEOCHANNELCC:
    return rheoMap[dacCcMap[channel]] >> 4;
    break;

  case CVRHEO:
    return rheoMap[dacCvMap[channel]] & 0x0F;
    break;

  case CCRHEO:
    return rheoMap[dacCcMap[channel]] & 0x0F;
    break;
  }
};

void OutputController::noteOff(uint8_t channel, uint8_t note, bool gateOff)
{
  //Serial.println("    OutputController -- off ch:"  + String(channel) + " nt: " + String(note));

  if (gateOff)
  {
    backplaneGPIO->digitalWrite(channel, LOW);
  }
  serialMidi->sendNoteOff(note, 64, globalObj->midiChannel[channel]);
  usbMIDI.sendNoteOff(note, 64, globalObj->midiChannel[channel]);
}

void OutputController::allNotesOff(uint8_t channel)
{
  //Serial.println("    OutputController -- all notes off ch:"  + String(channel) + " nt: "  + "\timer: " + String(debugTimer1) );

  backplaneGPIO->digitalWrite(channel, LOW);
  //this->cv2type[channel] = 0;
  //this->cv2amplitude[channel] = 0;
  //this->lfoRheoSet[channel] = 1;
  //this->clearVelocityOutput(channel);
}

bool OutputController::readClockPort()
{
  if (globalObj->clockPortDirection() == CLOCK_PORT_INPUT)
  {
    return digitalReadFast(CLOCK_PIN);
  }
  else
  {
    return 0;
  }
}

void OutputController::setClockOutput(bool value)
{
  //backplaneGPIO->digitalWrite(17, value);
  //digitalWriteFast(PIN_EXT_AD_1, value);
  if (globalObj->clockPortDirection() == CLOCK_PORT_OUTPUT)
  {
    digitalWriteFast(CLOCK_PIN, value);
  }

  clockValue = value;
  if (value == HIGH)
  {
    clockOutputTimer = 0;
  }
}

void OutputController::setGateOutputDebug(uint8_t index, bool value)
{
  backplaneGPIO->digitalWrite(index, value);
}

void OutputController::calibrationRoutine()
{
  for (int i = 0; i < 8; i++)
  {
    ad5676.setVoltage(i, globalObj->calibrationBuffer);
    delay(1);
  }
}
