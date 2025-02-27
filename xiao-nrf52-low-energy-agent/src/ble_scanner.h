#ifndef BLE_SCANNER_H_
#define BLE_SCANNER_H_


#include <bluefruit.h>

void init_scanner(void);
void start_scan(void);
void stop_scan(void);
void scan_callback(ble_gap_evt_adv_report_t* report);
void printUuid16List(uint8_t* buffer, uint8_t len);
void printUuid128List(uint8_t* buffer, uint8_t len);
#endif