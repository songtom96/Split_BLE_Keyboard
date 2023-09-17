#include <NimBLEDevice.h>

#define deviceName "TOMATOETOAST"
#define serviceID "TOMM"
#define charUuid "ACUL"

NimBLECharacteristic *pSecureCharacteristic = NULL;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  NimBLEDevice::init(deviceName);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
  NimBLEDevice::setSecurityAuth(true, true, true);
  //NimBLEDevice::setSecurityPasskey(123456);
  NimBLEServer *pServer = NimBLEDevice::createServer();
  NimBLEService *pService = pServer->createService(serviceID);
  pSecureCharacteristic = pService->createCharacteristic(charUuid, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);

  pSecureCharacteristic->setValue(1);

  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(serviceID);

  pService->start();
  pAdvertising->start();
}

void loop() {
  for (int i = 0; i < 5; i++) {
    pSecureCharacteristic->setValue(i);
    pSecureCharacteristic->notify();
    //Serial.println((unsigned int)&i,HEX);
    delay(1000);
  }
}
