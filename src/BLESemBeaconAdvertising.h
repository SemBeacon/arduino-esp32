/**
 * BLESemBeaconAdvertising.h
 * 	SemBeacon advertising wrapper to configure the advertisement data
 *  and scan response data.
 * 
 * @author Maxim Van de Wynckel
 * @version 1.0.0
 **/

#pragma once
#include "esp32-hal-log.h"
#include "BLEAdvertising.h"
#include "BLESemBeacon.h"
#include "BLEDevice.h"

class BLESemBeaconAdvertising {
private:
	BLESemBeacon*   		m_beacon;
	#ifdef CONFIG_BT_BLE_50_FEATURES_SUPPORTED
    BLEMultiAdvertising* 	m_advertising;
    #else
    BLEAdvertising*			m_advertising;
    #endif
    bool                    m_running;
    bool			        m_legacy = true;
protected:
    void            prepare();
public:
    /**
     * Get the beacon configuration used for the advertising
     * 
     * @return SemBeacon instance
     **/
    BLESemBeacon*   getBeacon();
    /**
     * Set the beacon configuration used for the advertising
     * 
     * @param beacon SemBeacon instance
     **/
	void            setBeacon(BLESemBeacon* beacon);
    /**
     * Start beacon advertising
     **/
    void            start();
    /**
     * Stop beacon advertising
     **/
    void            stop();
    /// @brief Check if currently advertising
    /// @return Boolean if advertising or not
    bool            isAdvertising();
    void            setLegacy(bool legacy);
    bool            isLegacy();
};
