//----------------------------------------------------------------------------------------------
// BSP : Seeed nRF52 Borads 1.1.8
// Board : Seeed nRF52 Borads / Seeed XIAO nRF52840 Sense
// important : need for Serial.print() <Arduino.h> <Adafruit_TinyUSB.h>
// \Arduino15\packages\Seeeduino\hardware\nrf52\1.1.1\libraries\Bluefruit52Lib\src
//----------------------------------------------------------------------------------------------

// 2024/03/18
// Central sketch : nRF52_XIAO_MTU_TEST_central.ino

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>   // For Serial.print()
#include <bluefruit.h>          // \Arduino15\packages\Seeeduino\hardware\nrf52\1.1.1\libraries\Bluefruit52Lib\src

#define dataNum 244             // MTU : default 20 --> 244(max247),  DataLength : default 27 --> 251
union unionData {               // Union for data type conversion
  uint32_t  dataBuff32[dataNum/4];
  uint16_t  dataBuff16[dataNum/2];
  uint8_t   dataBuff8[dataNum];
};
union unionData ud;
uint16_t conn;

// Custum Service and Characteristic
// 55c40000-8682-4dd1-be0c-40588193b485 for example
#define transmitService_UUID(val) (const uint8_t[]) { \
    0x85, 0xB4, 0x93, 0x81, 0x58, 0x40, 0x0C, 0xBE, \
    0xD1, 0x4D, (uint8_t)(val & 0xff), (uint8_t)(val >> 8), 0x00, 0x00, 0xC4, 0x55 }

BLEService        transmitService     (transmitService_UUID(0x0000));
BLECharacteristic dataCharacteristic   (transmitService_UUID(0x0030));

void setup()
{
  // Serial port initialization
  Serial.begin(115200);
//  while (!Serial);
  delay(2000);
  Serial.println("Start of initialization");
    
  // LED initialization   
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT); 
   
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  // Initialization of Bruefruit class
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.configUuid128Count(15);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setConnLedInterval(50);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Custom Service Settings
  transmitService.begin();

  dataCharacteristic.setProperties(CHR_PROPS_INDICATE);
  dataCharacteristic.setFixedLen(dataNum);
  dataCharacteristic.begin();

  // Advertisement Settings
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(transmitService);
  Bluefruit.ScanResponse.addName();
  
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setIntervalMS(20, 153);     // fast mode 20mS, slow mode 153mS
  Bluefruit.Advertising.setFastTimeout(30);         // fast mode 30 sec
  Bluefruit.Advertising.start(0);                   // 0 = Don't stop advertising after n seconds

  Serial.println("End of initialization");

  // Connecting to Central
  Serial.println("Connecting to Central ................");
  while(!Bluefruit.connected()) {
//    Serial.print("."); delay(100);
  }
  Serial.println();
  Serial.println("Connected to Central ................");
       
}

//*************************************************************************************************************
void loop()
{
  
//    Serial.println("Connecting to Central ........");
//    while(!Bluefruit.connected());
//    Serial.println("Connected to Central ........");  
    
    if (Bluefruit.connected())      // If connected to the central
    {
      digitalWrite(LED_GREEN, LOW);
/*
      for(int i = 0; i < dataNum / 4; i++) {
        ud.dataBuff32[i] = i;
        Serial.println(ud.dataBuff32[i]);         
      }
*/
      for(int i = 0; i < dataNum; i++) {
        ud.dataBuff8[i] = i;
        Serial.println(ud.dataBuff8[i]);
      }
           
      Serial.println("*********************************************");
      
      dataCharacteristic.indicate(ud.dataBuff8, dataNum);   // Data transfer

      digitalWrite(LED_GREEN, HIGH);
    }            
  delay(1000);   
}

//**************************************************************************************************************
// callback function
//*******************************************************************************************************
void connect_callback(uint16_t conn_handle)
{
  Serial.print("【connect_callback】 conn_Handle : ");
  Serial.println(conn_handle, HEX);
//  conn = conn_handle;

  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  Serial.println();
  // request PHY changed to 2MB (2Mbit/sec moduration) 1 --> 2
  Serial.print("Request to change PHY : "); Serial.print(connection->getPHY());
  connection->requestPHY();
  delayMicroseconds(1000000);  // delay a bit for all the request to complete
  Serial.print(" --> "); Serial.println(connection->getPHY());

  // request to update data length  27 --> 251
  Serial.print("Request to change Data Length : "); Serial.print(connection->getDataLength());
  connection->requestDataLengthUpdate();
  delayMicroseconds(1000000);  // delay a bit for all the request to complete
  Serial.print(" --> "); Serial.println(connection->getDataLength());
    
  // request mtu exchange 23 --> 247
  Serial.print("Request to change MTU : "); Serial.print(connection->getMtu());
  connection->requestMtuExchange(dataNum + 3);  // max 247
//  connection->requestMtuExchange(123);  // max 247
  delayMicroseconds(1000000);  // delay a bit for all the request to complete
  Serial.print(" --> "); Serial.println(connection->getMtu());

  // request connection interval  16(20mS) --> 16(20mS)
  Serial.print("Request to change Interval : "); Serial.print(connection->getConnectionInterval());
//  connection->requestConnectionParameter(16); // 20mS(in unit of 1.25) default 20mS
  delayMicroseconds(1000000);  // delay a bit for all the request to complete
  Serial.print(" --> "); Serial.println(connection->getConnectionInterval());

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("【connect_callback】 Connected to ");
  Serial.println(central_name);
}

//***********************************************************************************************
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.print("【disconnect_callback】 reason = 0x");
  Serial.println(reason, HEX);
}
