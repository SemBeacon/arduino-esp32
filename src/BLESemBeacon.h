/**
 * BLESemBeacon.h
 * 	This is the SemBeacon object containing the data for a SemBeacon.
 * 
 * @author Maxim Van de Wynckel
 * @version 1.0.0
 **/

#pragma once
#include <BLEUUID.h>
#include <esp_gap_ble_api.h>
#include "esp32-hal-log.h"

typedef struct {
	uint8_t			length;
	uint8_t			type;
} sembeacon_adv_data_t;

typedef struct {
	int major:	4;
	int minor: 	4;
} sembeacon_version_t;

/**
 * SemBeacon Flags
 **/
#define SEMBEACON_FLAG_HAS_POSITION		(0x01 << 0)
#define SEMBEACON_FLAG_PRIVATE			(0x01 << 1)
#define SEMBEACON_FLAG_MOVING 			(0x01 << 2)
#define SEMBEACON_FLAG_HAS_SYSTEM		(0x01 << 3)
#define SEMBEACON_FLAG_HAS_TELEMETRY	(0x01 << 4)
#define SEMBEACON_FLAG_RESERVED_1		(0x01 << 5)
#define SEMBEACON_FLAG_RESERVED_2		(0x01 << 6)
#define SEMBEACON_FLAG_RESERVED_3		(0x01 << 7)
#define SEMBEACON_FLAG_UNDEFINED		(0x00)

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

class BLESemBeacon {
private:
	// SemBeacon data
	BLEUUID namespaceId;
	uint32_t instanceId;
	int8_t signalPower;
	uint16_t manufacturerId;
	uint8_t flags;
	std::string uri;
	sembeacon_version_t version;
protected:
	std::string decodeURI(uint8_t* uri, uint8_t length);
	std::string	encodeURI(std::string uri, bool extended);
public:
	BLESemBeacon();
	std::string getAdvertisementData();
	std::string getManufacturerData();
	std::string getScanResponseData();
	std::string getExtendedAdvertisementData();
	uint16_t    getManufacturerId();
	void        setManufacturerId(uint16_t manufacturerId);
	BLEUUID 	getNamespaceId();
	void        setNamespaceId(BLEUUID uuid);
	uint32_t	getInstanceId();
	void		setInstanceId(uint32_t uuid);
	int8_t      getSignalPower();
	void        setSignalPower(int8_t rssi);
	/// @brief Get the resource URI
	/// @return Resource URI string
	std::string getResourceURI();
	/// @brief Set the resource URI
	/// @param uri Resource URI string
	void        setResourceURI(std::string uri);
	/// @brief Set the beacon flags
	/// @param flags SemBeacon Flags
	void 		setBeaconFlags(uint8_t flags);
	uint8_t		getBeaconFlags();
};
