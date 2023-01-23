#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEEddystoneURL.h>
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>
#include "esp32-hal-log.h"

#define BLE_SCAN_DURATION 5000
#define BLE_SCAN_WINDOW 99
#define BLE_SCAN_INTERVAL 100

BLEScan *scan;

#ifndef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
class AdvertisementCallback : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (advertisedDevice.haveManufacturerData() == true) {
            
        }
    }
};
#else
class AdvertisementCallback: public BLEExtAdvertisingCallbacks {
    void onResult(esp_ble_gap_ext_adv_reprot_t report) {
        if(report.event_type & ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY){
            // BLE 4.2

        } else {
            // BLE 5.0
            char buffer[512];
            sprintf(buffer, "%x", report.adv_data);
            Serial.println(buffer);
            log_buf_d((const uint8_t*) &buffer[0], report.adv_data_len);
        }
    }
};
#endif

void setup() {
    Serial.begin(115200);
    Serial.println("Scanning...");
    log_d("Scanning...");

    BLEDevice::init("");
    scan = BLEDevice::getScan();
    #ifdef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
    scan->setExtendedScanCallback(new AdvertisementCallback());
    scan->setExtScanParams();
    #else
    scan->setAdvertisedDeviceCallbacks(new AdvertisementCallback());
    scan->setActiveScan(true);
    scan->setInterval(BLE_SCAN_INTERVAL);
    scan->setWindow(BLE_SCAN_DURATION);
    #endif
}

void loop() {
    #ifndef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
    scan->start(BLE_SCAN_DURATION, nullptr, false);
    #else
    scan->startExtScan(BLE_SCAN_INTERVAL, BLE_SCAN_DURATION / 1000);
    #endif
}
