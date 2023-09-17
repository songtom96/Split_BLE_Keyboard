/*Declare Libraries
Utilizes the Blekeyboard library by T-vK on Github
Additionally uses NimBLEDevice for the BleKeyboard as well as server and client setup*/
//#include <BleKeyboard.h>
#include <NimBLEDevice.h>
//Defines
#define rows 5
#define columns 6

//BLE Variables

//Keyboard Variables
bool keyInRight[columns][rows];
uint8_t count = 0;
int adcBattery;
static uint32_t scanTime = 0; /** 0 = scan forever */
static bool doConnect = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE");

  NimBLEDevice::init("");
  NimBLEDevice::setSecurityAuth(false,false,true);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);

  NimBLEScan* pScan = NimBLEDevice::getScan();
  pScan->setInterval(45);
  pScan->setWindow(15);
  pScan->setActiveScan(true);
  pScan->start(scanTime);

}

void loop() {
  while(!doConnect){
    delay(1);
  }
  doConnect = false;
}

void test() {
  if (count < 1) {
    for (uint8_t i = 0; i < columns; i++) {
      for (uint8_t j = 0; j < rows; j++) {
        Serial.print(keyInRight[i][j]);
        Serial.print(", ");
      }
      Serial.print("\n");
    }
    count++;
  }
}
