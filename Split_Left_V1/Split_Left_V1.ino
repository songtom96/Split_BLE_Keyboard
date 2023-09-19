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
#include <BleKeyboard.h>

#define deviceName "ESP32Client"
#define serviceID "TOMM"
#define charUuid "ACUL"
#define rows 5
#define columns 6
#define numKeysPerSide rows *columns

NimBLEUUID serviceUuid(serviceID);                   //Set Service UUID
NimBLEClient *pClient = NULL;                        //Create Global pClient
NimBLERemoteService *pService = NULL;                //Create Global pService. Not really necessary so you could put this in the scanBLE().
NimBLERemoteCharacteristic *pCharacteristic = NULL;  //Create Global pCharacteristic to readValue(). The only one that really needs to be Global
bool isConnected = false;                            //Checks if the Right Keyboard was connected
bool keyInput[numKeysPerSide * 2];
int keyMap[] = {
  '~', '1', '2', '3', '4', '5',  //Left Keyboard
  KEY_TAB, 'q', 'w', 'e', 'r', 't',
  KEY_LEFT_SHIFT, 'a', 's', 'd', 'f', 'g',
  KEY_LEFT_CTRL, 'z', 'x', 'c', 'v', 'b',
  KEY_LEFT_ALT, KEY_CAPS_LOCK, KEY_DELETE, '{', ' ', 'A',


  '6', '7', '8', '9', '0', KEY_BACKSPACE,
  'y', 'u', 'i', 'o', 'p', ';',
  'h', 'j', 'k', 'l', '\'', KEY_RETURN,
  'n', 'm', ',', '.', '/', KEY_DELETE,
  ' ', '}', KEY_RIGHT_ALT, KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, 'A'
};

BleKeyboard bleKeyboard;

void scanBLE();
void rxParse();

class ClientCallbacks : public NimBLEClientCallbacks {
  void onConnect(NimBLEClient *pClient) {
    isConnected = true;
    Serial.println("Successfully Connected");
  };
  void onDisconnect(NimBLEClient *pClient) {
    isConnected = false;
    Serial.println("Device was Disconnected");
  };
  // uint32_t onPassKeyRequest() {
  //   Serial.println("Client Passkey Request");
  //   /** return the passkey to send to the server */
  //   /** Change this to be different from NimBLE_Secure_Server if you want to test what happens on key mismatch */
  //   return 123456;
  // };
};
static ClientCallbacks clientCB;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  NimBLEDevice::setSecurityAuth(true, true, true);
  //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY);

  bleKeyboard.begin();
}

void loop() {
  while (isConnected == false) {
    bleKeyboard.releaseAll();
    Serial.println("Scanning for new devices...");
    scanBLE();
  }
  rxParse();
  delay(1000);
}

void scanBLE() {
  NimBLEScan *pScan = NimBLEDevice::getScan();
  NimBLEScanResults results = pScan->start(5);        //scan for BLE devices for specified duration
  for (uint8_t i = 0; i < results.getCount(); i++) {  //check all returned BLE device to see if they are the Right keyboard
    NimBLEAdvertisedDevice device = results.getDevice(i);
    if (device.isAdvertisingService(serviceUuid)) {
      pClient = NimBLEDevice::createClient();
      pClient->setClientCallbacks(&clientCB, false);
      if (pClient->connect(&device)) {
        pService = pClient->getService(serviceUuid);
        if (pService != nullptr) {
          pCharacteristic = pService->getCharacteristic(charUuid);
          Serial.println("Connected");
        }
      } else {
        Serial.println("Failed to Connect");
      }
    }
  }
}

void rxParse() {
  int value;
  if (pCharacteristic->canRead()) {
    value = pCharacteristic->readValue<int>();
  }
  for (uint8_t i = 0; i < numKeysPerSide; i++) {
    if (value & 1 << i != 0) {
      keyInput[numKeysPerSide + i] = true;
    } else {
      keyInput[numKeysPerSide + i] = false;
    }
  }
}

void txKeyInput() {
  for(uint8_t i = 0; i < numKeysPerSide * 2; i++){
    if(keyInput[i] == true){
      bleKeyboard.press(keyMap[i]);
    }else{
      bleKeyboard.release(keyMap[i]);
    }
  }
}