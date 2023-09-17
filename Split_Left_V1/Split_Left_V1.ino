#include <NimBLEDevice.h>
#include <BleKeyboard.h>

#define deviceName "ESP32Client"
#define serviceID "TOMM"
#define charUuid "ACUL"

NimBLEUUID serviceUuid(serviceID);    //Set Service UUID 
NimBLEClient *pClient = NULL;   //Create Global pClient
NimBLERemoteService *pService = NULL;   //Create Global pService. Not really necessary so you could put this in the scanBLE().
NimBLERemoteCharacteristic *pCharacteristic = NULL;   //Create Global pCharacteristic to readValue(). The only one that really needs to be Global
bool isConnected = false;   //Checks if the Right Keyboard was connected

BleKeyboard bleKeyboard;

void scanBLE();

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
    Serial.println("Scanning for new devices...");
    scanBLE();
  }
  if (pCharacteristic->canRead()) {
    int value = pCharacteristic->readValue<int>();
    Serial.println(value);
    // if (bleKeyboard.isConnected()) {
    //   bleKeyboard.println(value);
    // }
  }
  delay(1000);
}

void scanBLE() {
  NimBLEScan *pScan = NimBLEDevice::getScan();
  NimBLEScanResults results = pScan->start(5);
  for (uint8_t i = 0; i < results.getCount(); i++) {
    NimBLEAdvertisedDevice device = results.getDevice(i);
    if (device.isAdvertisingService(serviceUuid)) {
      pClient = NimBLEDevice::createClient();
      pClient->setClientCallbacks(&clientCB, false);
      //pClient->setClientCallbacks(&clientCB, false);
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

void txParse() {
  int value;
  if (pCharacteristic->canRead()) {
    value = pCharacteristic->readValue<int>();
  }
}