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

#include "bdi/belief_base.h"
#include "bdi/event_base.h"
#include "bdi/plan_base.h"
#include "bdi/intention_base.h"
#include "../../functions.h"

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
  Body body_4;
  Context context_4;
  BeliefBase belief_base;
  EventBase event_base;
  PlanBase plan_base;
  IntentionBase intention_base;

public:
  AgentSettings()
  {
    belief_base = BeliefBase(2);
    event_base = EventBase(3);
    plan_base = PlanBase(5);
    intention_base = IntentionBase(10, 4);

    //--------------------------------------------------------------------------

    Belief belief_happy(0, nullptr, false);
    belief_base.add_belief(belief_happy);

    //--------------------------------------------------------------------------

    Belief belief_pressed(1, update_pressed, false);
    belief_base.add_belief(belief_pressed);

    //--------------------------------------------------------------------------

    Event event_2(EventOperator::GOAL_ADDITION, 2);
    event_base.add_event(event_2);

    //--------------------------------------------------------------------------

    Proposition prop_0(2);
    context_0 = Context(0);
    body_0 = Body(1);

    Proposition prop_0_body_0(0);
    BodyInstruction inst_0_0(BodyType::BELIEF, prop_0_body_0, EventOperator::BELIEF_ADDITION);
    body_0.add_instruction(inst_0_0);

    Plan plan_0(EventOperator::GOAL_ADDITION, prop_0, &context_0, &body_0);
    plan_base.add_plan(plan_0);

    //--------------------------------------------------------------------------

    Proposition prop_1(1);
    context_1 = Context(0);
    body_1 = Body(1);

    Proposition prop_1_body_0(3);
    BodyInstruction inst_0_1(BodyType::GOAL, prop_1_body_0, EventOperator::GOAL_ACHIEVE);
    body_1.add_instruction(inst_0_1);

    Plan plan_1(EventOperator::BELIEF_ADDITION, prop_1, &context_1, &body_1);
    plan_base.add_plan(plan_1);

    //--------------------------------------------------------------------------

    Proposition prop_2(3);
    context_2 = Context(0);
    body_2 = Body(1);

    Proposition prop_2_body_0(4);
    BodyInstruction inst_0_2(BodyType::ACTION, prop_2_body_0, action_swear);
    body_2.add_instruction(inst_0_2);

    Plan plan_2(EventOperator::GOAL_ADDITION, prop_2, &context_2, &body_2);
    plan_base.add_plan(plan_2);

    //--------------------------------------------------------------------------

    Proposition prop_3(0);
    context_3 = Context(0);
    body_3 = Body(1);

    Proposition prop_3_body_0(5);
    BodyInstruction inst_0_3(BodyType::GOAL, prop_3_body_0, EventOperator::GOAL_ACHIEVE);
    body_3.add_instruction(inst_0_3);

    Plan plan_3(EventOperator::BELIEF_ADDITION, prop_3, &context_3, &body_3);
    plan_base.add_plan(plan_3);

    //--------------------------------------------------------------------------

    Proposition prop_4(5);
    context_4 = Context(0);
    body_4 = Body(1);

    Proposition prop_4_body_0(6);
    BodyInstruction inst_0_4(BodyType::ACTION, prop_4_body_0, action_say_hello);
    body_4.add_instruction(inst_0_4);

    Plan plan_4(EventOperator::GOAL_ADDITION, prop_4, &context_4, &body_4);
    plan_base.add_plan(plan_4);
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