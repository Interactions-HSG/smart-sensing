//#define USE_BDI
#define USE_PROCEDURAL
#define PIN_DEBUG_SIGNAL_0 D10
#define PIN_DEBUG_SIGNAL_1 D9
#define PIN_WAKEUP D8
#define PIN_DONE D3
#define PIN_DFU_MODE_SET D2
#define PIN_BATTERY_VOLTAGE A0
#define PIN_DISABLE_LOW_POWER A1
#define PIN_LED_RED LED_RED
#define PIN_LED_BLUE LED_BLUE
#define PIN_LED_GREEN LED_GREEN
#define PIN_SHT41_POWER D2
#define PIN_ONE_WIRE_BUS D2
#define PIN_ONE_WIRE_POWER D3
#define DFU_DBL_RESET_DELAY             500

#include <Arduino.h>
//#include "SdFat.h"
//#include "Adafruit_SPIFlash.h"
#include "nrf_nvic.h"
#include "ble_uart_peripheral.h"
#include "ble_scanner.h"
#include "OneWire.h"
#include "DallasTemperature.h"


#if defined(CUSTOM_CS) && defined(CUSTOM_SPI)
  Adafruit_FlashTransport_SPI flashTransport(CUSTOM_CS, CUSTOM_SPI);
#elif defined(ARDUINO_ARCH_ESP32)
  Adafruit_FlashTransport_ESP32 flashTransport;
#else
  #if defined(EXTERNAL_FLASH_USE_QSPI)
    //Adafruit_FlashTransport_QSPI flashTransport;
  #elif defined(EXTERNAL_FLASH_USE_SPI)
    Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS, EXTERNAL_FLASH_USE_SPI);
  #else
    #error No QSPI/SPI flash are defined on your board variant.h !
  #endif
#endif

OneWire oneWire(PIN_ONE_WIRE_BUS);
DallasTemperature dsSensors(&oneWire);
// Number of temperature devices found
int numberOfDevices;
// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress; 

//Adafruit_SPIFlash flash(&flashTransport);

void deepSleep(void);
bool readTemperatures(uint16_t* data, int size);
void setupPins(void);
void powerOff(void);

void setup() {
  Serial.begin(115200);
  setupPins();
  digitalWrite(PIN_LED_GREEN, LOW);


  #ifdef MODE_PERIPHERAL
    // Start up the library
  dsSensors.begin();
  delay(50);  // Grab a count of devices on the wire
  numberOfDevices = dsSensors.getDeviceCount();
  if(numberOfDevices == 0){
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_RED, LOW);
    delay(500);
    powerOff();
  }
  init_peripheral();
  #else
  init_scanner();
  #endif
}

void setupPins()
{
  pinMode(PIN_DEBUG_SIGNAL_0, OUTPUT);
  digitalWrite(PIN_DEBUG_SIGNAL_0, HIGH);

  pinMode(PIN_DEBUG_SIGNAL_1, OUTPUT);
  pinMode(PIN_DISABLE_LOW_POWER, INPUT);

  pinMode(PIN_WAKEUP, INPUT);
  digitalWrite(PIN_DEBUG_SIGNAL_1, HIGH);

  pinMode(PIN_DONE, OUTPUT),
  digitalWrite(PIN_DONE, LOW);
}

#ifdef MODE_PERIPHERAL
bool readTemperatures(uint16_t* data, int size){
  dsSensors.requestTemperatures(); // Send the command to get temperatures
   numberOfDevices = dsSensors.getDeviceCount();
   if(numberOfDevices == 0 || numberOfDevices != size){
    return false;
   }
  // Loop through each device, print out temperature data
  int count = 0;
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(dsSensors.getAddress(tempDeviceAddress, i)){
      // Output the device ID
      Serial.print("Temperature for device: ");
      Serial.println(i,DEC);
      // Print the data
      float tempC = dsSensors.getTempC(tempDeviceAddress);
      *(data++) = (uint16_t)(tempC * 100);
      count++;
      Serial.printf("Temp %02X = %f\n", tempDeviceAddress[7], tempC);
    }
  }
  return count == size;
}
#endif

void powerOff(){
  digitalWrite(PIN_LED_GREEN, HIGH);
  digitalWrite(PIN_LED_BLUE, HIGH);
  digitalWrite(PIN_LED_RED, LOW);
  for(int i=0; i<10; i++){
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_DONE, HIGH);
    delay(500);
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_DONE, LOW);
    delay(500);
  }
}

void deepSleep(){
  //flashTransport.runCommand(0xB9);
  //flash.end();
  // 
  //waitForEvent();
  //digitalWrite(PIN_SHT41_POWER, LOW);
  //reset_shtx();
  NRF_POWER->GPREGRET = 0x6D;
  //systemOff(PIN_WAKEUP, HIGH);
  sd_power_gpregret_set(0, 0x6D);
 // systemOff(PIN_WAKEUP, HIGH);
  sd_power_system_off();
  //waitForEvent();
}

void loop() {
  #ifdef MODE_PERIPHERAL
  uint16_t pData[numberOfDevices]; 
  bool ok = readTemperatures(pData, numberOfDevices);
  Serial.printf("t1=%u t2=%u", pData[0], pData[1]);
  uint32_t data = pData[0] << 16 | pData[1];
  Serial.printf(" payload=%0X\n", data);
  start_adv(0xDA,data, 1, 1); //send daba for 30s fast adv, then rest 30 slow adv.
  delay(1100);
  powerOff();
  #else
    delay(1000);
  #endif
}

