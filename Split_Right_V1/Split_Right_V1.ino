/** Split_BLE_Keyboard
*
* Split wireless keyboard that uses two ESP32 with batteries. 
* Left keyboard acts as the master and the Right keyboard acts as the server, meaning only the left keyboard connects to the PC.
*
*   Created on Sept 16 2023
*       Author: Thomas Song
*/

//#include <Arduino.h>
#include <NimBLEDevice.h>

#define deviceName "TOMATOETOAST"
#define serviceID "TOMM"
#define charUuid "ACUL"
#define rows 5
#define columns 6

//Function declaration
void txBLE();
void keyScan();
void pinSetup();

//Global Variable declaration
NimBLECharacteristic *pSecureCharacteristic = NULL;
uint8_t columnPins[] = { 12, 14, 27, 26, 25, 33 };
uint8_t rowPins[] = { 19, 23, 18, 5, 17 };
int keytxData = 0;

void setup() {
  Serial.begin(115200);
  //MCU setup
  pinSetup();
  //Nimble setup
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
  keyScan();
  txBLE();
}
//Transmit the new keys being pressed or not pressed via BLE
void txBLE() {
  pSecureCharacteristic->setValue(keytxData);
  pSecureCharacteristic->notify();
}
//Scans the keyboard matrix to see which keys are pressed
void keyScan() {
  for (uint8_t i = 0; i < columns; i++) {
    uint8_t curColPin = columnPins[i];
    digitalWrite(curColPin, LOW);
    for (uint8_t j = 0; j < rows; j++) {
      if (digitalRead(rowPins[j]) == LOW) {
        keytxData = keytxData | 1 << (j * 6 + i);
      } else {
        keytxData = keytxData & (~(1 << (j * 6 + i)));
      }
    }
    digitalWrite(curColPin, HIGH);
  }
}
//Set up pins to be used for keyboard
void pinSetup() {
  for (uint8_t i = 0; i < columns; i++) {
    pinMode(columnPins[i], OUTPUT);
    digitalWrite(columnPins[i], HIGH);
  }
  for (uint8_t j = 0; j < rows; j++) {
    pinMode(rowPins[j], INPUT_PULLUP);
  }
}