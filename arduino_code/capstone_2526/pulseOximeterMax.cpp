#include "pulseOximeterMax.h"

pulseOximeterMax::pulseOximeterMax(){

}

void pulseOximeterMax::onBeatDetected()
{
    // Serial.println("Beat!");
}

bool pulseOximeterMax::initial_setup(){
    // Initialize the PulseOximeter instance
    if (!pox.begin()) {
        return false;
    }
    // pox.setSampleRate(100);
    // pox.setPulseWidth(MAX30102_PULSE_WIDTH_411);
    // pox.setADCRange(MAX30102_ADC_RANGE_4096);
    pox.setIRLedCurrent(MAX30102_LED_CURR_7_6MA);  // change this for the for ppg current, MAX30102_LED_CURR_50MA is 50mA, MAX30102_LED_CURR_7_6MA is 7.6mA
    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
    return true;
}


void pulseOximeterMax::update(){
    pox.update();
}

float pulseOximeterMax::getHeartRate(){
    return pox.getHeartRate();
}

uint8_t pulseOximeterMax::getSpO2(){
    return pox.getSpO2();
}

uint8_t pulseOximeterMax::getPPGSignal(){
    return pox.getRedLedCurrentBias();
}

// for the pox.rawIR and rawRed is modified the library 
// by adding rawIR and rawRed to get the raw data
float pulseOximeterMax::getRawIR(){
    return pox.rawIR;
}

float pulseOximeterMax::getRawRed(){
    return pox.rawRed;
}
