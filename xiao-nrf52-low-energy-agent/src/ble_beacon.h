#ifndef BLEBEACON_H_
#define BLEBEACON_H_


#include <bluefruit.h>

// Beacon uses the Manufacturer Specific Data field in the advertising
// packet, which means you must provide a valid Manufacturer ID. Update
// the field below to an appropriate value. For a list of valid IDs see:
// https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
// 0x004C is Apple
// 0x0822 is Adafruit
// 0x0059 is Nordic
#define MANUFACTURER_ID   0x0059

// "nRF Connect" app can be used to detect beacon
uint8_t beaconUuid[16] =
{
  0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
  0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0
};


void init(void);
void startAdv(void);

#endif