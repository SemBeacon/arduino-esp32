#define GPIO_DEEP_SLEEP_DURATION      100 // Sleep duration in ms
#define BLE_ADVERTISEMENT_INTERVAL    1
#define BEACON_UUID                   "77f340db-ac0d-20e8-aa3a-f656a29f236c"

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
  beacon.setInstanceId(0x48d787c1);                           // Instance Identifier
  beacon.setResourceURI("https://bit.ly/3JsEnF9");            // URI to the resource
  beacon.setBeaconFlags(SEMBEACON_FLAG_HAS_POSITION & SEMBEACON_FLAG_HAS_SYSTEM);

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
