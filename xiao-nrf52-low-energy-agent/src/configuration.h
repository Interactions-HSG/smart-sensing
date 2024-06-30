/*
 * AgentSpeak code:
 *
 * // file used to test translator
 * 
 * !start.
 * 
 * +!start <- +happy.
 * 
 * +pressed <- !!growl.
 * 
 * +!growl <- swear.
 * 
 * +happy <- !!hello.
 * 
 * +!hello <- say_hello.
 */ 

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "belief_base.h"
#include "event_base.h"
#include "plan_base.h"
#include "intention_base.h"
#include "functions.h"

class AgentSettings
{
private:
  Body body_0;
  Context context_0;
  Body body_1;
  Context context_1;
  Body body_2;
  Context context_2;
  Body body_3;
  Context context_3;
  BeliefBase belief_base;
  EventBase event_base;
  PlanBase plan_base;
  IntentionBase intention_base;

public:
  AgentSettings()
  {
    belief_base = BeliefBase(3);
    event_base = EventBase(3);
    plan_base = PlanBase(4);
    intention_base = IntentionBase(10, 4);

    //--------------------------------------------------------------------------

    Belief belief_energy_buffer_high(0, update_energy_buffer_high, false);
    belief_base.add_belief(belief_energy_buffer_high);

    //--------------------------------------------------------------------------

    Belief belief_energy_buffer_low(1, update_energy_buffer_low, false);
    belief_base.add_belief(belief_energy_buffer_low);

    //--------------------------------------------------------------------------

    Belief belief_measurement_required(2, nullptr, false);
    belief_base.add_belief(belief_measurement_required);

    //--------------------------------------------------------------------------

    Event event_3(EventOperator::GOAL_ADDITION, 3);
    event_base.add_event(event_3);

    //--------------------------------------------------------------------------

    Proposition prop_0(3);
    context_0 = Context(1);
    body_0 = Body(3);

    Proposition prop_0_energy_buffer_high(0);
    ContextCondition cond_0_0(prop_0_energy_buffer_high);
    context_0.add_context(cond_0_0);

    Proposition prop_0_body_0(4);
    BodyInstruction inst_0_0(BodyType::ACTION, prop_0_body_0, action_read_sensor);
    body_0.add_instruction(inst_0_0);

    Proposition prop_0_body_1(5);
    BodyInstruction inst_1_0(BodyType::ACTION, prop_0_body_1, action_transmit_data);
    body_0.add_instruction(inst_1_0);

    Proposition prop_0_body_2(6);
    BodyInstruction inst_2_0(BodyType::ACTION, prop_0_body_2, action_sleep);
    body_0.add_instruction(inst_2_0);

    Plan plan_0(EventOperator::GOAL_ADDITION, prop_0, &context_0, &body_0);
    plan_base.add_plan(plan_0);

    //--------------------------------------------------------------------------

    Proposition prop_1(3);
    context_1 = Context(1);
    body_1 = Body(2);

    Proposition prop_1_energy_buffer_low(1);
    ContextCondition cond_1_0(prop_1_energy_buffer_low);
    context_1.add_context(cond_1_0);

    Proposition prop_1_body_0(2);
    BodyInstruction inst_0_1(BodyType::BELIEF, prop_1_body_0, EventOperator::BELIEF_ADDITION);
    body_1.add_instruction(inst_0_1);

    Proposition prop_1_body_1(6);
    BodyInstruction inst_1_1(BodyType::ACTION, prop_1_body_1, action_sleep);
    body_1.add_instruction(inst_1_1);

    Plan plan_1(EventOperator::GOAL_ADDITION, prop_1, &context_1, &body_1);
    plan_base.add_plan(plan_1);

    //--------------------------------------------------------------------------

    Proposition prop_2(0);
    context_2 = Context(1);
    body_2 = Body(1);

    Proposition prop_2_measurement_required(2);
    ContextCondition cond_2_0(prop_2_measurement_required);
    context_2.add_context(cond_2_0);

    Proposition prop_2_body_0(3);
    BodyInstruction inst_0_2(BodyType::GOAL, prop_2_body_0, EventOperator::GOAL_ACHIEVE);
    body_2.add_instruction(inst_0_2);

    Plan plan_2(EventOperator::BELIEF_ADDITION, prop_2, &context_2, &body_2);
    plan_base.add_plan(plan_2);

    //--------------------------------------------------------------------------

    Proposition prop_3(1);
    context_3 = Context(0);
    body_3 = Body(1);

    Proposition prop_3_body_0(6);
    BodyInstruction inst_0_3(BodyType::ACTION, prop_3_body_0, action_sleep);
    body_3.add_instruction(inst_0_3);

    Plan plan_3(EventOperator::BELIEF_ADDITION, prop_3, &context_3, &body_3);
    plan_base.add_plan(plan_3);
  }

  BeliefBase * get_belief_base()
  {
    return &belief_base;
  }

  EventBase * get_event_base()
  {
    return &event_base;
  }

  PlanBase * get_plan_base()
  {
    return &plan_base;
  }

  IntentionBase * get_intention_base()
  {
    return &intention_base;
  }
};

#endif /*CONFIGURATION_H_ */