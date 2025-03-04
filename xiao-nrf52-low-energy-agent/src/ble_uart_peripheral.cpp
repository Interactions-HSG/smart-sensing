#include "ble_uart_peripheral.h"
#include <bluefruit.h>

#define MANUFACTURER_ID   0x0059
#define PIN_DEBUG_SIGNAL_1 9

// "nRF Connect" app can be used to detect beacon
uint8_t uuid[16] =
{
  0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
  0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0
};

BLEUart bleuart; // uart over ble

//static BLEBeacon beacon(uuid, 0x0102, 0x0304, -54);

void init_peripheral(void) 
{
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin();
  Bluefruit.setName("AgentBlue01");
  // off Blue LED for lowest power consumption
  Bluefruit.autoConnLed(true);
  Bluefruit.setTxPower(8);    // Check bluefruit.h for supported values
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  Bluefruit.Advertising.setStopCallback(adv_stop_callback);
  // Manufacturer ID is required for Manufacturer Specific Data
  //beacon.setManufacturer(MANUFACTURER_ID);
  bleuart.begin();
}



void start_adv(uint8_t key, uint32_t value, uint16_t battery, uint16_t fast_timeout, uint16_t slow_timeout)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  // Advertising with only board ID
  struct ATTR_PACKED {
    uint16_t mfr_id;
    //uint8_t  field_len;
    //uint8_t field_key;
    uint32_t field_value;
    uint16_t field_battery;
  } mfr_adv;

  mfr_adv.mfr_id = UUID16_COMPANY_ID_ADAFRUIT;
  //mfr_adv.field_len = 9;
  //mfr_adv.field_key = key; // board id
  mfr_adv.field_value = value;
  mfr_adv.field_battery = battery;

  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.Advertising.addManufacturerData(&mfr_adv, sizeof(mfr_adv));
  // Add name to advertising, since there is enough room
  //Bluefruit.Advertising.addName();
  Bluefruit.ScanResponse.addName();
  Bluefruit.ScanResponse.addManufacturerData(&mfr_adv, sizeof(mfr_adv));
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 160);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(fast_timeout);      // number of seconds in fast mode
  //digitalWrite(PIN_DEBUG_SIGNAL_1, HIGH);
  Bluefruit.Advertising.start(slow_timeout);
  //delay(5);
  //Bluefruit.Advertising.stop();
  //Serial.println("Advertisting");                
}

void stop_adv()
{
  Bluefruit.Advertising.stop();
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  //Serial.print("Connected to ");
  //Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  //Serial.println();
  //Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}

void adv_stop_callback(){
  //digitalWrite(PIN_DEBUG_SIGNAL_1, LOW);
}

void do_rx_tx()
{
  // Forward data from HW Serial to BLEUART
  while (Serial.available())
  {
    // Delay to wait for enough input, since we have a limited transmission buffer
    delay(2);

    uint8_t buf[64];
    int count = Serial.readBytes(buf, sizeof(buf));
    bleuart.write( buf, count );
  }

  // Forward from BLEUART to HW Serial
  while ( bleuart.available() )
  {
    uint8_t ch;
    ch = (uint8_t) bleuart.read();
    Serial.write(ch);
  }
}