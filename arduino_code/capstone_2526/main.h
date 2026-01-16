#pragma once
#include <Arduino.h>
#include <Wire.h>

#define DEVICE_ONE 1

// ================== BUILD MODE ==================
#define DEBUG_MODE

// ================== MODULE ENABLE FLAGS ==================
// Set to 1 to enable, 0 to disable
#define ENABLE_MAX30102   1
#define ENABLE_MPU6050    1
#define ENABLE_BLE        1
#define ENABLE_OLED       1
#define ENABLE_BATTERY    1
#define ENABLE_INACTIVITY 1

// ================== TEST FLAGS ==================
#define TEST_MAX
// #define TEST_BLE
// #define TEST_MPU
#define TEST_BATT

// ================== I2C ==================
#define SDA_PIN 8
#define SCL_PIN 9

// ================== BATTERY ==================
#define MAX_BATT 4.2
#define MAX_INPUT 3.3
#define VOLT_DIVIDER 2
#define batt_reader 6

// ================== FILTER ==================
#define LOW_PASS_ALPHA 0.2

// ================== FUNCTION DECLARATIONS ==================
float lowPassFilter(float input, float alpha);
void max_update_loop();
void max_loop();
// void mpu_loop();
void ble_loop();
void batt_level();
void oled_loop();
void check_inactivity();
