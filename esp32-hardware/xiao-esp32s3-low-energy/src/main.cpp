#include <Arduino.h>
#include "esp_err.h"
#include "esp_sleep.h"

int led = BUILTIN_LED;

#define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;


/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup(){
  Serial.begin(9600);
  delay(1000); //Take some time to open up the Serial Monitor
  pinMode(led, OUTPUT);
  //digitalWrite(led, HIGH);   // turn the LED on 
  //delay(1000);               // wait for a second
  //digitalWrite(led, LOW);    // turn the LED off
  //delay(1000);               // wait for a second

  //Increment boot number and print it every reboot
  ++bootCount;
  //Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  //print_wakeup_reason();

  //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

  //If you were to use ext1, you would use it like
  //esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

  //Go to sleep now
  //Serial.println("Going to sleep now");
  //Serial.flush(); 
  //esp_deep_sleep_start();
  //Serial.println("This will never be printed");
}

void loop(){
  digitalWrite(led, HIGH);   // turn the LED on 
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off
  delay(1000);               // wait for a second
  Serial.println("Bleep");
}