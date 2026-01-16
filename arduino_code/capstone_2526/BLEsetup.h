/*********
  Rui Santos
  Complete instructions at https://RandomNerdTutorials.com/esp32-ble-server-client/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#pragma once
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

//BLE server name
#define bleServerName "FOVIAN"
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"

// ---------- GLOBAL FLAGS ----------
extern bool deviceConnected;

// ---------- CHARACTERISTICS ----------
extern BLECharacteristic sensorCharacteristics;
extern BLE2902 sensorDescriptor;

// Data packet
struct sensorPacket{
  uint8_t header;
  uint16_t heart_rate;
  uint8_t oximetry;
  uint8_t ppg;
  uint8_t step;
  uint8_t inactivity;
  uint8_t fallDetected;
  uint8_t battery;
  uint8_t checksum;
};

class BLEConnection{
  public:
    BLEConnection();
    void initial_setup();
    void send_data(float fHeartRate, uint8_t iOximetry, uint8_t iPPG, uint8_t iStep, uint8_t iInactivity, uint8_t iFallDetected, uint8_t iBattery);

  private:
    uint8_t calCheckSum(uint8_t*, uint8_t);

};