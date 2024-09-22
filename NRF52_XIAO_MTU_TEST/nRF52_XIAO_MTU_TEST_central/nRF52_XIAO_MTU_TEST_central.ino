//----------------------------------------------------------------------------------------------
// BSP : Seeed nRF52 Borads 1.1.8
// Board : Seeed nRF52 Borads / Seeed XIAO nRF52840
// important : need for Serial.print() <Arduino.h> <Adafruit_TinyUSB.h>
// \Arduino15\packages\Seeeduino\hardware\nrf52\1.1.1\libraries\Bluefruit52Lib\src
//----------------------------------------------------------------------------------------------

// 2024/03/18
// Peripheral sketch : nRF52_XIAO_MTU_TEST_peripheral.ino

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>     // For Serial.print()
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSerif9pt7b.h"
#include <bluefruit.h>            // \Arduino15\packages\Seeeduino\hardware\nrf52\1.1.1\libraries\Bluefruit52Lib\src

#define LOOP_TIME    1            // Loop time [sec]

#define dataNum 244               // max MTU = 247
union unionData {                 // Union for data type conversion
  uint32_t  dataBuff32[dataNum/4];
  uint16_t  dataBuff16[dataNum/2];
  uint8_t   dataBuff8[dataNum];
};
union unionData ud;
uint8_t dataBuff[dataNum];

char* versionNumber = "0.99";   // Central sketch version number
bool indicateFlag = false;
bool connectedFlag = false;
uint16_t receiveNumber = 0;

// Instances of the class
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Custum Service and Characteristic
// 55c40000-8682-4dd1-be0c-40588193b485 for example
#define receiveService_UUID(val) (const uint8_t[]) { \
    0x85, 0xB4, 0x93, 0x81, 0x58, 0x40, 0x0C, 0xBE, \
    0xD1, 0x4D, (uint8_t)(val & 0xff), (uint8_t)(val >> 8), 0x00, 0x00, 0xC4, 0x55 }

BLEClientService        receiveService     (receiveService_UUID(0x0000));
BLEClientCharacteristic dataCharacteristic   (receiveService_UUID(0x0030));

void setup()
{
  // Serial port initialization
  Serial.begin(115200);
  while (!Serial);
  delay(2000);

  // LED intialization
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
/*
  // Initialization of display SSD1306
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("Initialization failure of SSD1306");
  }
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 15);
  display.println("MTU TEST");
  display.setCursor(0, 31);
  display.println("BLE Central");
  display.setCursor(0, 47);
  display.print("Ver   : ");
  display.println(versionNumber);
  display.display();
  delay(5000);
  display.clearDisplay();
*/
  // Initialization of Bruefruit
  Bluefruit.configCentralBandwidth(BANDWIDTH_MAX);  
  Bluefruit.begin(1, 1);
  Bluefruit.setName("XIAO BLE Central");

  // Custom Service Settings
  receiveService.begin();
  dataCharacteristic.setIndicateCallback(data_indicate_callback);
//  dataCharacteristic.setNotifyCallback(data_indicate_callback);  
  dataCharacteristic.begin();

  // Blue LED blinking interval setting
  Bluefruit.setConnLedInterval(100);

  // Callbacks
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);
  Bluefruit.Central.setConnectCallback(connect_callback);

  // Scanner settings
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setIntervalMS(100, 50);     // interval:100mS  window:50mS  
  Bluefruit.Scanner.filterUuid(receiveService.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);                   // Continuous Scanning

  Serial.println("End of initialization");

    // Wait for response from peripheral
  Serial.println("Connecting to Peripheral ....");
  while (!Bluefruit.connected()) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("Connected to Peripheral");

}

//******************************************************************************************
void loop()
{
  uint32_t timestamp = millis();
/*
  // Wait for response from peripheral
  Serial.println("Connecting to Peripheral ....");
  while (!Bluefruit.connected()) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("Connected to Peripheral");
*/
  if(indicateFlag == true & Bluefruit.connected())
  {
    digitalWrite(LED_GREEN, LOW);

    // open dataBUff to receive the next data
    memcpy(ud.dataBuff8, dataBuff, sizeof(dataBuff) / sizeof(dataBuff[0]));
    indicateFlag = false;

    Serial.print(receiveNumber);
    Serial.print("  ");
    Serial.println(ud.dataBuff16[0]);
    receiveNumber++;
    
    // Restore and display incoming data
    for(int i = 0; i < dataNum; i++) {
      Serial.println(ud.dataBuff8[i]);   
    }
    Serial.println("***************************************");
/*
    display.clearDisplay();
    display.setCursor(0, 15);
    display.print("mS "); display.print(mS);
    display.setCursor(0, 31);
    display.print("uS "); display.print(uS);
    display.setCursor(0, 47);
    display.print("minute "); display.print(mS/60000.0, 3);  
    display.display();
*/ 
    digitalWrite(LED_GREEN, HIGH);
    indicateFlag = false;
  }
//  while(millis() - timestamp < 1000 * LOOP_TIME);
}

//*****************************************************************************************************
// Callback invoked when scanner pick up an advertising data
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  Bluefruit.Central.connect(report);
}

//****************************************************************************************************:
// Callback invoked when an connection is established
void connect_callback(uint16_t conn_handle)
{
  connectedFlag = false;
  
  if ( !receiveService.discover(conn_handle) )
  {
    Serial.println("【connect_callback】 Service not find disconnect!");
    Bluefruit.disconnect(conn_handle);
    return;
  }
  Serial.println("【connect_callback】 Find Service");
  
  if ( !dataCharacteristic.discover() )
  {
    Serial.println("【connect_callback】 Characteristic not find disconnect!");
    Bluefruit.disconnect(conn_handle);
    return;
  }
  Serial.println("【connect_callback】 Find Characteristic");
  
  if ( !dataCharacteristic.enableIndicate() )
//  if ( !dataCharacteristic.enableNotify() )
  {
    Serial.println("【connect_callback】 Couldn't enable indicate for data");
    Bluefruit.disconnect(conn_handle);
    return;
  }
  Serial.println("【connect_callback】 enable indicate");

  connectedFlag = true;
}

//**********************************************************************************************************
// Callback invoked when an connection is broken
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.print("【disconnect_callback】 Disconnected, reason = 0x"); Serial.println(reason, HEX);
}

//*********************************************************************************************************
// Callback invoked when a notifyication is received
void data_indicate_callback(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len)
{
  Serial.print("【indicate_callback】 indicate_callback, len = "); Serial.println(len);

  while(!(indicateFlag == false | connectedFlag == true)) {
    Serial.println("wait for indocateFlag clear"); // wait for indicateFlag clear
  }
  
  digitalWrite(LED_RED, LOW);

  memcpy(dataBuff, data, len);
  
  delay(1);   // for LED
  digitalWrite(LED_RED, HIGH);

  indicateFlag = true;
}
