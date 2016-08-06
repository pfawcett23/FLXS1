#ifndef _leds_h_
#define _leds_h_
#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <FastLED.h>
#include "global.h"
#include "Sequencer.h"

#define NUM_LEDS 20
#define DATA_PIN 5

class LEDArray
{
  public:
    LEDArray();
    void initialize(Sequencer *sequenceArray);
    void loop(uint16_t frequency);
    void fadeall();

  private:
    Sequencer *sequenceArray;
    elapsedMicros pixelTimer;

    CRGB leds[NUM_LEDS];
    //uint8_t ledMapping[NUM_LEDS] =   {3,2,1,0,8,7,6,5,13,12,11,10,18,17,16,15,4,9,14,19};
    uint8_t ledMapping[NUM_LEDS] = {1,2,3,4,6,7,8,9,11,12,13,14,16,17,18,19,0,5,10,15};
};

#endif
