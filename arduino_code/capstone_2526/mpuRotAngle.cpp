#include "mpuRotAngle.h"

mpuRotAngle::mpuRotAngle()
  : mpu(), ax(0), ay(0), az(0), gx(0), gy(0), gz(0),
    accMagnitude(0), angleChange(0), stepCount(0), lastStepTime(0), prev_acc(1.0),
    fall(false), trigger1(false), trigger2(false), trigger3(false),
    trigger1_time(0), trigger2_time(0), trigger3_time(0),
    ax_offset(0), ay_offset(0), az_offset(0),
    angleX(0), angleY(0), angleZ(0) {}

bool mpuRotAngle::initial_setup() {
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    return false;
  }

  Serial.println("MPU6050 connected, calibrating...");
  startCalibration();
  Serial.println("Calibration done!");
  return true;
}

void mpuRotAngle::startCalibration() {
  long sum_ax = 0, sum_ay = 0, sum_az = 0;
  const int samples = 200;
  for (int i = 0; i < samples; i++) {
    mpu.getAcceleration(&accX_raw, &accY_raw, &accZ_raw);
    sum_ax += accX_raw;
    sum_ay += accY_raw;
    sum_az += accZ_raw;
    delay(5);
  }
  ax_offset = sum_ax / (float)samples;
  ay_offset = sum_ay / (float)samples;
  az_offset = sum_az / (float)samples;
  prev_acc = 1.0;  // initial stationary G
}

void mpuRotAngle::update() {
  // Read raw data
  mpu.getAcceleration(&accX_raw, &accY_raw, &accZ_raw);
  mpu.getRotation(&gyroX_raw, &gyroY_raw, &gyroZ_raw);

  // Convert to G and dps
  ax = (accX_raw - ax_offset) / 16384.0f;
  ay = (accY_raw - ay_offset) / 16384.0f;
  az = (accZ_raw - az_offset) / 16384.0f;

  gx = gyroX_raw / 131.0f;
  gy = gyroY_raw / 131.0f;
  gz = gyroZ_raw / 131.0f;

  // Magnitude and total angular velocity
  accMagnitude = sqrt(ax * ax + ay * ay + az * az);
  angleChange = sqrt(gx * gx + gy * gy + gz * gz);

  // Optional: compute angles (complementary filter)
  static float dt = 0.01f;
  static unsigned long lastTime = millis();
  unsigned long now = millis();
  dt = (now - lastTime) / 1000.0f;
  lastTime = now;

  // Roll/pitch angles
  float rollAcc = atan2(ay, az) * 57.29578f;
  float pitchAcc = atan2(-ax, sqrt(ay * ay + az * az)) * 57.29578f;

  static const float alpha = 0.98f;
  angleX = alpha * (angleX + gx * dt) + (1 - alpha) * rollAcc;
  angleY = alpha * (angleY + gy * dt) + (1 - alpha) * pitchAcc;
  angleZ += gz * dt;  // Yaw integrated from gyro
}

void mpuRotAngle::detectStep() {
  unsigned long now = millis();
  if (accMagnitude > 1.5 && prev_acc <= 1.5) {
    if (now - lastStepTime > STEP_DEBOUNCE) {
      stepCount++;
      lastStepTime = now;
    }
  }
  prev_acc = accMagnitude;
}

bool mpuRotAngle::detectFall() {
  unsigned long now = millis();

  // Stage 1: free fall
  if (accMagnitude <= 0.33 && !trigger1 && !trigger2) {
    trigger1 = true;
    trigger1_time = now;
    Serial.println("Free fall detected!");
  }

  // Stage 2: impact
  if (trigger1 && accMagnitude >= 1.35) {
    trigger2 = true;
    trigger2_time = now;
    trigger1 = false;
    Serial.println("Impact detected!");
  }

  // Final fall trigger: ignore rotation
  if (trigger2) {
    trigger2 = false;
    return true;  // Return true only at the moment of impact
  }
  return false;
}


void mpuRotAngle::resetFallDetection() {
  fall = false;
  trigger1 = false;
  trigger2 = false;
  trigger3 = false;
  trigger1_time = trigger2_time = trigger3_time = 0;
}
