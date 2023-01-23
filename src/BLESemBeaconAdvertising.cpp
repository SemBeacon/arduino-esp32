#include "BLESemBeaconAdvertising.h"

BLESemBeacon* BLESemBeaconAdvertising::getBeacon() {
    return m_beacon;
}

void BLESemBeaconAdvertising::setBeacon(BLESemBeacon* beacon) {
    m_beacon = beacon;
    this->prepare();
}

void BLESemBeaconAdvertising::prepare() {
    m_running = false;

    // Construct BLE 4.0 beacon service data
    BLEAdvertisementData advertisementData = BLEAdvertisementData();
    advertisementData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    advertisementData.addData(m_beacon->getAdvertisementData());

    // Construct BLE 4.0 scan response data
    BLEAdvertisementData scanResponseData = BLEAdvertisementData();
    scanResponseData.addData(m_beacon->getScanResponseData()); 

    // Construct BLE 5.0 extended advertisement data
    std::string extendedAdvertisement = m_beacon->getExtendedAdvertisementData();

    #ifdef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
    log_d("Using BLE v5.0 for advertising");
    m_advertising = new BLEMultiAdvertising(isLegacy() ? 2 : 1);

    int i = 0;
    if (isLegacy()) {
        log_d("Including legacy advertisement in extended advertisement");
        esp_ble_gap_ext_adv_params_t legacy_adv_params = {
            .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_SCAN,
            .interval_min = 0x30,
            .interval_max = 0x30,
            .channel_map = ADV_CHNL_ALL,
            .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
            .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
            .primary_phy = ESP_BLE_GAP_PHY_1M,
            .max_skip = 0,
            .secondary_phy = ESP_BLE_GAP_PHY_2M,
            .sid = i,
            .scan_req_notif = false,
        };

        m_advertising->setAdvertisingParams(i, &legacy_adv_params);
        m_advertising->setAdvertisingData(i, advertisementData.getPayload().length(), (const uint8_t*)advertisementData.getPayload().data());
        m_advertising->setScanRspData(i, scanResponseData.getPayload().length(), (const uint8_t*)scanResponseData.getPayload().data());
        m_advertising->setDuration(i);   
        i++;
    }

    esp_ble_gap_ext_adv_params_t ext_adv_params = {
        .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED,
        .interval_min = 0x40,
        .interval_max = 0x40,
        .channel_map = ADV_CHNL_ALL,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
        .primary_phy = ESP_BLE_GAP_PHY_1M,
        .max_skip = 0,
        .secondary_phy = ESP_BLE_GAP_PHY_CODED,
        .sid = i,
        .scan_req_notif = false,
    };

    m_advertising->setAdvertisingParams(i, &ext_adv_params);
    m_advertising->setAdvertisingData(i, extendedAdvertisement.length(), (const uint8_t*)extendedAdvertisement.data());
    m_advertising->setDuration(i);
    #else
    log_d("Using BLE v4.2 for advertising");
    m_advertising = BLEDevice::getAdvertising();
    m_advertising->setAdvertisementData(advertisementData);
    m_advertising->setScanResponse(true);
    m_advertising->setScanResponseData(scanResponseData);

    // Disable connection to the beacon
    m_advertising->setAdvertisementType(ADV_TYPE_SCAN_IND);
    #endif
}

void BLESemBeaconAdvertising::start() {
    // Start the advertising
    log_d("Starting SemBeacon advertisement!");
    m_running = true;
    #ifdef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
    m_advertising->start(isLegacy() ? 2 : 1, 0);
    #else
    m_advertising->start();
    #endif
}

void BLESemBeaconAdvertising::stop() {
    // Stop the advertising
    log_d("Stopping SemBeacon advertisement!");
    m_running = false;
    #ifdef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
    m_advertising->stop(1, (const uint8_t*) 0);
    if (isLegacy()) {
        m_advertising->stop(1, (const uint8_t*) 1);
    }
    #else
    m_advertising->stop();
    #endif
}

bool BLESemBeaconAdvertising::isAdvertising() {
    return m_running;
}

void BLESemBeaconAdvertising::setLegacy(bool legacy) {
    this->m_legacy = legacy;
}

bool BLESemBeaconAdvertising::isLegacy() {
    return this->m_legacy;
}