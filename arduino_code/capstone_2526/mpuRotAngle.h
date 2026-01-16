#ifndef MPU_ROT_ANGLE_H
#define MPU_ROT_ANGLE_H

#include <Arduino.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"

// Step detection constants
#define STEP_DEBOUNCE 300  // ms

class mpuRotAngle {
public:
    mpuRotAngle();

    bool initial_setup();
    void update();           // Read raw data, calculate magnitude
    void detectStep();
    bool detectFall();
    void resetFallDetection();

    int getStepCount() { return stepCount; }
    float getAngleX() { return angleX; }
    float getAngleY() { return angleY; }
    float getAngleZ() { return angleZ; }
    float getAccMagnitude() { return accMagnitude; }
    float getAngleChange() { return angleChange; }

private:
    MPU6050 mpu;

    // Raw data
    int16_t accX_raw, accY_raw, accZ_raw;
    int16_t gyroX_raw, gyroY_raw, gyroZ_raw;

    // Scaled data
    float ax, ay, az;
    float gx, gy, gz;
    float accMagnitude;
    float angleChange;

    // Orientation angles (optional)
    float angleX, angleY, angleZ;

    // Step detection
    float prev_acc;
    unsigned long lastStepTime;
    int stepCount;

    // Fall detection
    bool fall;
    bool trigger1, trigger2, trigger3;
    unsigned long trigger1_time, trigger2_time, trigger3_time;

    // Calibration offsets
    float ax_offset, ay_offset, az_offset;

    void startCalibration();
};

#endif
