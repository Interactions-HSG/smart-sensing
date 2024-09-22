//#define USE_BDI
#define USE_PROCEDURAL
#define PIN_DEBUG_SIGNAL_0 D10
#define PIN_DEBUG_SIGNAL_1 D9
#define PIN_WAKEUP D8
#define PIN_DONE D3
#define PIN_DFU_MODE_SET D2
#define PIN_BATTERY_VOLTAGE A0
#define PIN_DISABLE_LOW_POWER A1
#define PIN_SHT41_POWER D2
#define DFU_DBL_RESET_DELAY             500

#include <Arduino.h>
//#include "SdFat.h"
//#include "Adafruit_SPIFlash.h"
#include "nrf_nvic.h"


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

//Adafruit_SPIFlash flash(&flashTransport);

void deepSleep(void);

void setup() {
  pinMode(PIN_DEBUG_SIGNAL_0, OUTPUT);
  digitalWrite(PIN_DEBUG_SIGNAL_0, HIGH);
  pinMode(PIN_DEBUG_SIGNAL_1, OUTPUT);
  pinMode(PIN_DISABLE_LOW_POWER, INPUT);
  pinMode(PIN_WAKEUP, INPUT);
  digitalWrite(PIN_DEBUG_SIGNAL_1, HIGH);
  //flash.begin();
  delay(100);
  digitalWrite(PIN_DEBUG_SIGNAL_0, LOW);
  digitalWrite(PIN_DEBUG_SIGNAL_1, LOW);
  if(digitalRead(PIN_DISABLE_LOW_POWER) != HIGH){
    //suspendLoop();
    deepSleep();
  }
}


void deepSleep(){
  if(digitalRead(PIN_DISABLE_LOW_POWER) == HIGH){
    return;
  }
  //flashTransport.runCommand(0xB9);
  //flash.end();
  // 
  //waitForEvent();
  //digitalWrite(PIN_SHT41_POWER, LOW);
  //reset_shtx();
  NRF_POWER->GPREGRET = 0x6D;
  //systemOff(PIN_WAKEUP, HIGH);
  //sd_power_gpregret_set(0, 0x6D);
  systemOff(PIN_WAKEUP, HIGH);
  //sd_power_system_off();
  //waitForEvent();
}

void loop() {
  deepSleep();
  delay(10000);
}

