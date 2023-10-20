<div style="margin-left: auto; margin-right: auto; width: 80%">
    <img src="https://sembeacon.org/images/logo.svg" width="100%">
</div>

# SemBeacon Arduino ESP32

## About
This repository contains the source code for scanning and advertising SemBeacon packets over
Bluetooth Low Energy 4.x and 5.x using an ESP32 and the Arduino-ESP32 library.

## Specification
The full specification that this library implements can be found at:
https://github.com/SemBeacon/specification

## Usage
In order to broadcast a beacon that is semantically represented using the following
RDF description:

```turtle
:BEACON_08 a sosa:FeatureOfInterest, ogc:SpatialObject, poso:RFLandmark, poso:BluetoothBeacon, sembeacon:SemBeacon;
    rdfs:label "BEACON_08";
    dcmi:created "2023-10-13T16:17:21.950Z"^^xsd:dateTime;
    poso:hasPosition [
        a geo:Point, poso:AbsolutePosition;
        ogc:asWKT "POINT Z(4.392253994600526 50.82057562786381 93.5999999962747)"^^ogc:wktLiteral;
        ogc:coordinateDimension 3;
        ogc:spatialDimension 3;
        ogc:dimension 3;
        dcmi:created "2023-10-13T16:17:21.950Z"^^xsd:dateTime;
        schema:longitude "4.392253994600526"^^xsd:double;
        poso:xAxisValue "4.392253994600526"^^xsd:double;
        schema:latitude "50.82057562786381"^^xsd:double;
        poso:yAxisValue "50.82057562786381"^^xsd:double;
        schema:elevation "93.5999999962747"^^xsd:double;
        poso:zAxisValue "93.5999999962747"^^xsd:double
    ];
    posoc:hasReferenceRSSI [
        a poso:RelativeSignalStrength;
        poso:hasRSS [
            a qudt:QuantityValue;
            qudt:unit unit:DeciB_M;
            qudt:numericValue -56
        ];
        poso:hasRelativeDistance [
            a qudt:QuantityValue;
            qudt:unit unit:M;
            qudt:numericValue 1
        ]
    ];
    hardware:macAddress "a8:3d:86:62:98:9b";
    ogc:sfWithin :pl9_3_corridor;
    sembeacon:namespace :pl9_3;
    sembeacon:shortResourceURI "https://bit.ly/3JsEnF9"^^xsd:anyURI;
    sembeacon:instanceId "9c7ce6fc"^^xsd:hexBinary.
```

You should configure the beacon as:

```cpp
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
  // Choose 0x004C for Apple Ltd. Note that this will
  // not automatically be detected on iOS
  // beacon.setManufacturerId(0x004C);
  // OR
  // beacon.setManufacturerId(0x4C00, true);
  beacon.setManufacturerId(0xFFFF);                           // Manufacturer of the beacon
  beacon.setSignalPower(-36);                                 // RSSI at 1m distance
  beacon.setNamespaceId(BLEUUID(BEACON_UUID));                // Namespace UUID
  beacon.setInstanceId(0x9c7ce6fc);                           // Instance Identifier
  // OR
  // beacon.setInstanceId(0xfce67c9c, true);

  beacon.setResourceURI("https://bit.ly/3JsEnF9");            // URI to the resource
  beacon.setBeaconFlags(SEMBEACON_FLAG_HAS_POSITION | SEMBEACON_FLAG_HAS_SYSTEM);

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
```

## Directories
- `/src`: Source directory of the library
- `/examples`: Examples of implementations with this library

## License
