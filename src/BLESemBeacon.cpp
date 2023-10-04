/**
 * BLESemBeacon.cpp
 * 	This is the SemBeacon object containing the data for a SemBeacon.
 * 	Encoding and decoding aspect based on https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/src/BLEEddystoneURL.h
 * 
 * @author Maxim Van de Wynckel
 * @version 1.0.0
 **/

#include "sdkconfig.h"
#if defined(CONFIG_BLUEDROID_ENABLED)
#include <string.h>
#include "BLESemBeacon.h"
#include "esp32-hal-log.h"
#include <string>

static const char *sembeacon_uri_prefix_subs[] = {
	// -- Eddystone URL -- //
	"http://www.",
	"https://www.",
	"http://",
	"https://",
	"urn:uuid:",
	// -- SemBeacon BLE 5.0 -- //
	"urn:",
	"tag:",
	"http://purl.org/",
	"https://purl.org/",
	"http://w3id.org/",
	"https://w3id.org/",
	NULL};

static const char *sembeacon_uri_suffix_subs[] = {
	// -- Eddystone URL -- //
	".com/",
	".org/",
	".edu/",
	".net/",
	".info/",
	".biz/",
	".gov/",
	".com",
	".org",
	".edu",
	".net",
	".info",
	".biz",
	".gov",
	// -- SemBeacon BLE 5.0 -- //
	".rdf#",
	".ttl#",
	".rdf",
	".ttl",
	".jsonld",
	NULL};

static int string_begin_with(const char *str, const char *prefix)
{
	int prefix_len = strlen(prefix);
	if (strncmp(prefix, str, prefix_len) == 0)
	{
		return prefix_len;
	}
	return 0;
}

BLESemBeacon::BLESemBeacon()
{
	this->manufacturerId = 0x4C00;				   // Default manufacturer
	this->instanceId = 0;						   // Default instance ID
	this->signalPower = -56;					   // Signal power at 1m
	this->flags = SEMBEACON_FLAG_UNDEFINED;

	this->version = {
		.major = 0x1,
		.minor = 0x0
	};
}

std::string BLESemBeacon::decodeURI(uint8_t *uri, uint8_t length)
{ 
	std::string decodedURI = "";
	uint8_t sub_idx = 0;
	while (sembeacon_uri_prefix_subs[sub_idx] != NULL)
	{
		if (uri[0] == sub_idx)
		{
			decodedURI += sembeacon_uri_prefix_subs[sub_idx];
		}
		sub_idx++;
	}
	for (int i = 1; i < length; i++)
	{
		if (uri[i] > 33 && uri[i] < 127)
		{
			decodedURI += uri[i];
		}
		else if (uri[i] <= 33)
		{
			sub_idx = 0;
			while (sembeacon_uri_suffix_subs[sub_idx] != NULL)
			{
				if (uri[i] == sub_idx)
				{
					decodedURI += sembeacon_uri_suffix_subs[sub_idx];
				}
				sub_idx++;
			}
		}
	}
	return decodedURI;
}

std::string BLESemBeacon::encodeURI(std::string uri, bool extended)
{
	int scheme_len, ext_len = 1, i, idx, uri_idx;
	int uri_len = uri.length();
	char *ret_data = (char *)calloc(1, uri_len);

	i = 0, idx = 0, uri_idx = 0;

	// Replace URI prefix
	scheme_len = 0;
	while (sembeacon_uri_prefix_subs[i] != NULL)
	{
		if ((scheme_len = string_begin_with(uri.c_str(), sembeacon_uri_prefix_subs[i])) > 0)
		{
			ret_data[idx] = i;
			idx++;
			uri_idx += scheme_len;
			break;
		}
		i++;

		// SemBeacon extended encoding
		if (i > 5 && extended) {
			break;
		}
	}

	while (uri_idx < uri_len)
	{
		i = 0;
		ret_data[idx] = uri[uri_idx];
		ext_len = 1;
		while (sembeacon_uri_suffix_subs[i] != NULL)
		{
			if ((ext_len = string_begin_with(&uri[uri_idx], sembeacon_uri_suffix_subs[i])) > 0)
			{
				ret_data[idx] = i;
				break;
			}
			else
			{
				ext_len = 1;
			}
			i++;

			// SemBeacon extended encoding
			if (i > 14 && extended) {
				break;
			}
		}
		uri_idx += ext_len;
		idx++;
	}
	return std::string(ret_data, idx);
}

std::string BLESemBeacon::getManufacturerData()
{
	struct {
		uint16_t 	manufacturerId;		// Manufacturer identifier
		uint16_t 	beaconCode;			// AltBeacon code
		uint8_t  	namespaceId[16];	// Namespace identifier
		uint32_t	instanceId;			// Instance identifier
		int8_t   	signalPower;		// TX Power @ 1m distance
		uint8_t  	flags;				// SemBeacon flags
	} __attribute__((packed)) m_adv_data;

	m_adv_data.manufacturerId = ENDIAN_CHANGE_U16(this->manufacturerId);
	m_adv_data.beaconCode = ENDIAN_CHANGE_U16(0xBEAC); // AltBeacon code
	m_adv_data.instanceId = this->instanceId;
	m_adv_data.signalPower = this->signalPower;
	m_adv_data.flags = this->flags;
	memset(m_adv_data.namespaceId, 0, sizeof(m_adv_data.namespaceId));
	memcpy(m_adv_data.namespaceId, BLEUUID(this->namespaceId.getNative()->uuid.uuid128, 16, true).getNative()->uuid.uuid128, 16);
	return std::string((char *)&m_adv_data, sizeof(m_adv_data));
}

std::string BLESemBeacon::getAdvertisementData()
{
	log_d("Constructing SemBeacon advertisement data ...");
	std::string data = getManufacturerData();
	char cdata[2];
	cdata[0] = data.length() + 1;
	cdata[1] = ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE; // 0xFF
	return std::string(cdata, 2) + data;
}

std::string BLESemBeacon::getExtendedAdvertisementData()
{
	log_d("Constructing SemBeacon extended advertisement data ...");
	struct {
		uint8_t  	namespaceId[16];	// Namespace identifier
		uint32_t	instanceId;			// Instance identifier
		int8_t   	signalPower;		// TX Power @ 1m distance
		sembeacon_version_t	version;	// SemBeacon version
		uint8_t  	flags;				// SemBeacon flags
		uint8_t		uri[129];			// Prefix + Encoded URI
	} __attribute__((packed)) m_ext_adv_data;
	m_ext_adv_data.instanceId = this->instanceId;
	m_ext_adv_data.signalPower = this->signalPower;
	m_ext_adv_data.flags = this->flags;
	m_ext_adv_data.version = this->version;

	memset(m_ext_adv_data.namespaceId, 0, sizeof(m_ext_adv_data.namespaceId));
	memcpy(m_ext_adv_data.namespaceId, BLEUUID(this->namespaceId.getNative()->uuid.uuid128, 16, true).getNative()->uuid.uuid128, 16);

	std::string uriData = encodeURI(this->uri, true);
	memset(m_ext_adv_data.uri, 0, sizeof(m_ext_adv_data.uri));
	memcpy(m_ext_adv_data.uri, uriData.data(), std::min(uriData.length(), (unsigned int) 129));

	std::string data = std::string((char *)&m_ext_adv_data, sizeof(m_ext_adv_data));
	char cdata[6];
	cdata[0] = data.length() + 5 - (129 - std::min(uriData.length(), (unsigned int) 129));
	cdata[1] = 0x20;
	cdata[2] = 0x00;	// 32-bit UUID
	cdata[3] = 0xAC;
	cdata[4] = 0xBE;
	cdata[5] = 0x00;
	return std::string(cdata, 6) + data;
}

std::string BLESemBeacon::getScanResponseData()
{
	log_d("Constructing SemBeacon scan response data ...");
	struct {
		uint8_t  	frame;				// Eddystone Compatible Frame
		int8_t   	signalPower;		// TX power @ 0m
		uint8_t		uri[18];			// Prefix + Encoded URI
	} __attribute__((packed)) m_scan_rsp_data;
	m_scan_rsp_data.frame = 0x10;	   // Eddystone compatible URL frame
	m_scan_rsp_data.signalPower = this->signalPower + 41; // Signal power at 0m

	std::string uriData = encodeURI(this->uri, false);
	memset(m_scan_rsp_data.uri, 0, sizeof(m_scan_rsp_data.uri));
	memcpy(m_scan_rsp_data.uri, uriData.data(), std::min(uriData.length(), (unsigned int) 18));

	std::string data = std::string((char *)&m_scan_rsp_data, sizeof(m_scan_rsp_data));
	char cdata[4];
	cdata[0] = data.length() + 3 - (18 - std::min(uriData.length(), (unsigned int) 18));
	cdata[1] = 0x16;
	cdata[2] = 0xAA;
	cdata[3] = 0xFE;
	return std::string(cdata, 4) + data;
}

uint16_t BLESemBeacon::getManufacturerId()
{
	return this->manufacturerId;
}

void BLESemBeacon::setManufacturerId(uint16_t manufacturerId)
{
	this->manufacturerId = manufacturerId;
}

int8_t BLESemBeacon::getSignalPower()
{
	return this->signalPower;
}

void BLESemBeacon::setSignalPower(int8_t rssi)
{
	this->signalPower = rssi;
}

uint32_t BLESemBeacon::getInstanceId()
{
	return this->instanceId;
}

void BLESemBeacon::setInstanceId(uint32_t id)
{
	this->instanceId = id;
}

BLEUUID BLESemBeacon::getNamespaceId()
{
	return this->namespaceId;
}

void BLESemBeacon::setNamespaceId(BLEUUID uuid)
{
	this->namespaceId = uuid;
}

std::string BLESemBeacon::getResourceURI()
{
	return this->uri;
}

void BLESemBeacon::setResourceURI(std::string uri)
{
	this->uri = uri;
}

uint8_t BLESemBeacon::getBeaconFlags()
{
	return this->flags;
}

void BLESemBeacon::setBeaconFlags(uint8_t flags)
{
	this->flags = flags;
}

#endif
