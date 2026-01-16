#include "BLEsetup.h"

// Setup callbacks onConnect and onDisconnect
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println("Device connected.");
  }
  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("Device disconnected.");
    pServer->startAdvertising();
  }
};

extern bool deviceConnected = false;
BLECharacteristic sensorCharacteristics("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);
BLE2902 sensorDescriptor;

BLEConnection::BLEConnection(){}

// FIXED: Handles data SENT FROM the phone to the ESP32
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      // FIX: Use Arduino String to match your library's return type
      String rxValue = pCharacteristic->getValue().c_str(); 
      
      if (rxValue.length() > 0) {
        Serial.print("BLE Received: ");
        Serial.println(rxValue);

        // Check for 'R' or 'RESET'
        if (rxValue.indexOf("R") >= 0 || rxValue.indexOf("RESET") >= 0) {
          extern volatile bool bleResetTriggered; 
          bleResetTriggered = true; 
        }
      }
    }
};

void BLEConnection::initial_setup() {
  BLEDevice::init(bleServerName);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *sensorsService = pServer->createService(SERVICE_UUID);

  // Setup Characteristic with Callbacks
  sensorsService->addCharacteristic(&sensorCharacteristics);
  sensorCharacteristics.setCallbacks(new MyCallbacks()); 
  sensorCharacteristics.addDescriptor(new BLE2902());

  sensorsService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("BLE System Initialized. Waiting for connection...");
}

uint8_t BLEConnection::calCheckSum(uint8_t* data, uint8_t len){
  uint8_t sum = 0;
  for (int i = 0; i < len; i++) sum ^= data[i];
  return sum;
}

void BLEConnection::send_data(float fHeartRate, uint8_t iOximetry, uint8_t iPPG, uint8_t iStep, uint8_t iInactivity, uint8_t iFallDetected, uint8_t iBattery) {
  if (!deviceConnected) return;
  sensorPacket pkt;
  pkt.header = 0x11;
  pkt.heart_rate = (uint16_t)(fHeartRate * 100);
  pkt.oximetry = iOximetry;
  pkt.ppg = iPPG;
  pkt.step = iStep;
  pkt.inactivity = iInactivity;
  pkt.fallDetected = iFallDetected;
  pkt.battery = iBattery;
  pkt.checksum = calCheckSum((uint8_t*)&pkt, sizeof(pkt) - 1);

  sensorCharacteristics.setValue((uint8_t*)&pkt, sizeof(pkt));
  sensorCharacteristics.notify();
}