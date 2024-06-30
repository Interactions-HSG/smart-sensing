#include <Arduino.h>
#include <agent.h>
#include "configuration.h"
#include "functions.h"

#define PIN_DEBUG_SIGNAL 10
#define PIN_BATTERY_VOLTAGE A0
#define PIN_SENSOR A1

AgentSettings agent_settings;
BeliefBase * beliefs = agent_settings.get_belief_base();
EventBase * events = agent_settings.get_event_base();
PlanBase * plans = agent_settings.get_plan_base();
IntentionBase * intentions = agent_settings.get_intention_base();
Agent agent(beliefs, events, plans, intentions);



void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_DEBUG_SIGNAL, OUTPUT);
  pinMode(PIN_BATTERY_VOLTAGE, INPUT);
  pinMode(PIN_SENSOR, INPUT);
}

void evaluate(){
  bool energy_ok = update_energy_buffer_high(true);
  if(energy_ok || true){
    action_read_sensor();
    action_transmit_data();
    action_sleep();
  }else{
    action_sleep();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(PIN_DEBUG_SIGNAL, HIGH);
  //agent.run();
  evaluate();
  delay(500);
  digitalWrite(PIN_DEBUG_SIGNAL, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(2000);
}

