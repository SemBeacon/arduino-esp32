#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"

#define BLE_ADV_INTERVAL              100
#define BLE_ADV_RESPONSE              true

BLEAdvertising *advertising;

#define BEACON_UUID                   "8ec76ea3-6668-48da-9866-75be8bc86f4d"

void createBeacon() {
  BLEBeacon beacon = BLEBeacon();
  beacon.setManufacturerId(0x4C00);
  beacon.setProximityUUID(BLEUUID(BEACON_UUID));
  beacon.setMajor(10001);
  beacon.setMinor(10002);
  beacon.setSignalPower(-56);
  BLEAdvertisementData advertisementData = BLEAdvertisementData();
  BLEAdvertisementData scanResponseData = BLEAdvertisementData();
  
  advertisementData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
  
  std::string strServiceData = "";
  
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += beacon.getData(); 
  advertisementData.addData(strServiceData);
  
  advertising->setAdvertisementData(advertisementData);
  #ifdef BLE_ADV_RESPONSE
  advertising->setScanResponse(true);
  advertising->setScanResponseData(scanResponseData);
  advertising->setAdvertisementType(ADV_TYPE_SCAN_IND);  
  #else
  advertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);  
  #endif
}

void setup() {
  // Create the BLE Device
  BLEDevice::init("");
  advertising = BLEDevice::getAdvertising();
  
  createBeacon();
}

void loop() {
  // Start advertising
  advertising->start();
  delay(100);
  advertising->stop();
  esp_deep_sleep(1000LL * BLE_ADV_INTERVAL);
}
