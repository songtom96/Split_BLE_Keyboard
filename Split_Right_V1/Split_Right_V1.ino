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
void pinSetup();

//Global Variable declaration
NimBLECharacteristic *pSecureCharacteristic = NULL;
uint8_t columnPins[] = { 12, 14, 27, 26, 25, 33 };
uint8_t rowPins[] = { 22, 19, 23, 18, 5 };
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
  // for (int i = 0; i < 5; i++) {
  //   pSecureCharacteristic->setValue(i);
  //   pSecureCharacteristic->notify();
  //   //Serial.println((unsigned int)&i,HEX);
  //   delay(1000);
  // }
  int test = 0;
  test = 1 << 2 | test;
  test = test | 1 << 1;
  Serial.println(test);
  delay(1000);
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
      if (digitalRead(rowPins[j])) {
        keytxData = keytxData | 1 << (j * 5 + i);
      } else {
        keytxData = keytxData | 0 << (j * 5 + i);
      }
    }
    digitalWrite(curColPin, HIGH);
  }
}
//Set up pins to be used for keyboard
void pinSetup() {
  for (uint8_t i = 0; i < columns; i++) {
    pinMode(columnPins[i], OUTPUT);
  }
  for (uint8_t j = 0; j < rows; j++) {
    pinMode(rowPins[j], INPUT_PULLUP);
  }
}