#include "ble_beacon.h"
#include <bluefruit.h>

static BLEBeacon beacon(beaconUuid, 0x0102, 0x0304, -54);

void init(void) 
{
  Bluefruit.begin();
  // off Blue LED for lowest power consumption
  Bluefruit.autoConnLed(false);
  Bluefruit.setTxPower(0);    // Check bluefruit.h for supported values
  // Manufacturer ID is required for Manufacturer Specific Data
  beacon.setManufacturer(MANUFACTURER_ID);
}

void startAdv(void)
{  
  Bluefruit.Advertising.setBeacon(beacon);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(1600, 1600);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}