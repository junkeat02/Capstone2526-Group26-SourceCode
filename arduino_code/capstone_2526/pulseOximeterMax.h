#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "MAX30102_PulseOximeter.h"

class pulseOximeterMax{
  private:
    static void onBeatDetected();
    PulseOximeter pox;
  public:
    pulseOximeterMax(); 
    bool initial_setup();
    void update();
    float getHeartRate();
    uint8_t getSpO2();
    uint8_t getPPGSignal();
    float getRawIR();
    float getRawRed();
};