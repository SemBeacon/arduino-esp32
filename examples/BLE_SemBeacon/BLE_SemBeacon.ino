#define GPIO_DEEP_SLEEP_DURATION      100 // Sleep duration in ms
#define BLE_ADVERTISEMENT_INTERVAL    1
#define BEACON_UUID                   "fda50693-a4e2-4fb1-afcf-c6eb07647825"

/* Uncomment the following two lines to disable BLE5 */
#include "sdkconfig.h"
#undef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
/* ------------------------------------------------- */

#include <BLESemBeacon.h>
#include <BLESemBeaconAdvertising.h>

#include "BLEDevice.h"
#include "esp_sleep.h"

BLESemBeaconAdvertising *advertising;

void createBeacon() {
  BLESemBeacon beacon = BLESemBeacon();
  beacon.setManufacturerId(0x4c00);                           // Manufacturer of the beacon
  beacon.setSignalPower(-56);                                 // RSSI at 1m distance
  beacon.setNamespaceId(BLEUUID(BEACON_UUID));                // Namespace UUID
  beacon.setInstanceId(0x3C0B1D8B);                           // Instance Identifier
  beacon.setResourceURI("https://tinyurl.com/57mbbx2w");      // URI to the resource
  
  advertising->setBeacon(&beacon);
}

void setup() {
  // Create the BLE Device
  BLEDevice::init("");
  advertising = new BLESemBeaconAdvertising();
  // Create the beacon data
  createBeacon();
}

void loop() {
  // Start advertising
  advertising->start();
  delay(100);
  advertising->stop();
  esp_deep_sleep(1000LL * GPIO_DEEP_SLEEP_DURATION);
}
