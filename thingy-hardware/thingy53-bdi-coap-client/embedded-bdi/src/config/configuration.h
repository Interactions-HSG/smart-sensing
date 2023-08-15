/*
 * AgentSpeak code:
 *
 * +user_turn_on <- !!preserve_light.
 * 
 * +!preserve_light : bright_inside & user_turn_on <- !!preserve_light.
 * 
 * +!preserve_light : dark_inside & user_turn_on <- !brighten ; !!preserve_light.
 * 
 * -!preserve_light : user_turn_on <- !!preserve_light.
 * 
 * +sunny_outside: user_turn_on <- +eco_mode_available.
 * 
 * +cloudy_outside: user_turn_on <- -eco_mode_available ; +standard_mode_available.
 * 
 * +night_outside: user_turn_on <- -eco_mode_available ; +standard_mode_available.
 * 
 * +!brighten: eco_mode_available <- turn_off_lights ; raise_blinds;  -eco_mode_available.
 * 
 * +!brighten: standard_mode_available <- turn_on_lights; -standard_mode_available.
 * 
 * +user_turn_off <- !!preserve_dark.
 * 
 * +!preserve_dark: night_outside & user_turn_off <- lower_blinds ; !!preserve_dark.
 * 
 * +!preserve_dark: bright_inside & user_turn_off  <- turn_off_lights; lower_blinds ; !!preserve_dark.
 * 
 * -!preserve_dark: user_turn_off <- !!preserve_dark.
 */ 

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "bdi/belief_base.h"
#include "bdi/event_base.h"
#include "bdi/plan_base.h"
#include "bdi/intention_base.h"
#include "../../data/functions.h"

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
  Body body_5;
  Context context_5;
  Body body_6;
  Context context_6;
  Body body_7;
  Context context_7;
  Body body_8;
  Context context_8;
  Body body_9;
  Context context_9;
  Body body_10;
  Context context_10;
  Body body_11;
  Context context_11;
  Body body_12;
  Context context_12;
  BeliefBase belief_base;
  EventBase event_base;
  PlanBase plan_base;
  IntentionBase intention_base;

public:
  AgentSettings()
  {
    belief_base = BeliefBase(9);
    event_base = EventBase(6);
    plan_base = PlanBase(13);
    intention_base = IntentionBase(10, 4);

    //--------------------------------------------------------------------------

    Belief belief_bright_inside(0, update_bright_inside, false);
    belief_base.add_belief(belief_bright_inside);

    //--------------------------------------------------------------------------

    Belief belief_cloudy_outside(1, update_cloudy_outside, false);
    belief_base.add_belief(belief_cloudy_outside);

    //--------------------------------------------------------------------------

    Belief belief_dark_inside(2, update_dark_inside, false);
    belief_base.add_belief(belief_dark_inside);

    //--------------------------------------------------------------------------

    Belief belief_eco_mode_available(3, nullptr, false);
    belief_base.add_belief(belief_eco_mode_available);

    //--------------------------------------------------------------------------

    Belief belief_night_outside(4, update_night_outside, false);
    belief_base.add_belief(belief_night_outside);

    //--------------------------------------------------------------------------

    Belief belief_standard_mode_available(5, nullptr, false);
    belief_base.add_belief(belief_standard_mode_available);

    //--------------------------------------------------------------------------

    Belief belief_sunny_outside(6, update_sunny_outside, false);
    belief_base.add_belief(belief_sunny_outside);

    //--------------------------------------------------------------------------

    Belief belief_user_turn_off(7, update_user_turn_off, false);
    belief_base.add_belief(belief_user_turn_off);

    //--------------------------------------------------------------------------

    Belief belief_user_turn_on(8, update_user_turn_on, false);
    belief_base.add_belief(belief_user_turn_on);

    //--------------------------------------------------------------------------

    Proposition prop_0(8);
    context_0 = Context(0);
    body_0 = Body(1);

    Proposition prop_0_body_0(9);
    BodyInstruction inst_0_0(BodyType::GOAL, prop_0_body_0, EventOperator::GOAL_ACHIEVE);
    body_0.add_instruction(inst_0_0);

    Plan plan_0(EventOperator::BELIEF_ADDITION, prop_0, &context_0, &body_0);
    plan_base.add_plan(plan_0);

    //--------------------------------------------------------------------------

    Proposition prop_1(9);
    context_1 = Context(2);
    body_1 = Body(1);

    Proposition prop_1_bright_inside(0);
    ContextCondition cond_1_0(prop_1_bright_inside);
    context_1.add_context(cond_1_0);

    Proposition prop_1_user_turn_on(8);
    ContextCondition cond_1_1(prop_1_user_turn_on);
    context_1.add_context(cond_1_1);

    Proposition prop_1_body_0(9);
    BodyInstruction inst_0_1(BodyType::GOAL, prop_1_body_0, EventOperator::GOAL_ACHIEVE);
    body_1.add_instruction(inst_0_1);

    Plan plan_1(EventOperator::GOAL_ADDITION, prop_1, &context_1, &body_1);
    plan_base.add_plan(plan_1);

    //--------------------------------------------------------------------------

    Proposition prop_2(9);
    context_2 = Context(2);
    body_2 = Body(2);

    Proposition prop_2_dark_inside(2);
    ContextCondition cond_2_0(prop_2_dark_inside);
    context_2.add_context(cond_2_0);

    Proposition prop_2_user_turn_on(8);
    ContextCondition cond_2_1(prop_2_user_turn_on);
    context_2.add_context(cond_2_1);

    Proposition prop_2_body_0(10);
    BodyInstruction inst_0_2(BodyType::GOAL, prop_2_body_0, EventOperator::GOAL_ADDITION);
    body_2.add_instruction(inst_0_2);

    Proposition prop_2_body_1(9);
    BodyInstruction inst_1_2(BodyType::GOAL, prop_2_body_1, EventOperator::GOAL_ACHIEVE);
    body_2.add_instruction(inst_1_2);

    Plan plan_2(EventOperator::GOAL_ADDITION, prop_2, &context_2, &body_2);
    plan_base.add_plan(plan_2);

    //--------------------------------------------------------------------------

    Proposition prop_3(9);
    context_3 = Context(1);
    body_3 = Body(1);

    Proposition prop_3_user_turn_on(8);
    ContextCondition cond_3_0(prop_3_user_turn_on);
    context_3.add_context(cond_3_0);

    Proposition prop_3_body_0(9);
    BodyInstruction inst_0_3(BodyType::GOAL, prop_3_body_0, EventOperator::GOAL_ACHIEVE);
    body_3.add_instruction(inst_0_3);

    Plan plan_3(EventOperator::GOAL_DELETION, prop_3, &context_3, &body_3);
    plan_base.add_plan(plan_3);

    //--------------------------------------------------------------------------

    Proposition prop_4(6);
    context_4 = Context(1);
    body_4 = Body(1);

    Proposition prop_4_user_turn_on(8);
    ContextCondition cond_4_0(prop_4_user_turn_on);
    context_4.add_context(cond_4_0);

    Proposition prop_4_body_0(3);
    BodyInstruction inst_0_4(BodyType::BELIEF, prop_4_body_0, EventOperator::BELIEF_ADDITION);
    body_4.add_instruction(inst_0_4);

    Plan plan_4(EventOperator::BELIEF_ADDITION, prop_4, &context_4, &body_4);
    plan_base.add_plan(plan_4);

    //--------------------------------------------------------------------------

    Proposition prop_5(1);
    context_5 = Context(1);
    body_5 = Body(2);

    Proposition prop_5_user_turn_on(8);
    ContextCondition cond_5_0(prop_5_user_turn_on);
    context_5.add_context(cond_5_0);

    Proposition prop_5_body_0(3);
    BodyInstruction inst_0_5(BodyType::BELIEF, prop_5_body_0, EventOperator::BELIEF_DELETION);
    body_5.add_instruction(inst_0_5);

    Proposition prop_5_body_1(5);
    BodyInstruction inst_1_5(BodyType::BELIEF, prop_5_body_1, EventOperator::BELIEF_ADDITION);
    body_5.add_instruction(inst_1_5);

    Plan plan_5(EventOperator::BELIEF_ADDITION, prop_5, &context_5, &body_5);
    plan_base.add_plan(plan_5);

    //--------------------------------------------------------------------------

    Proposition prop_6(4);
    context_6 = Context(1);
    body_6 = Body(2);

    Proposition prop_6_user_turn_on(8);
    ContextCondition cond_6_0(prop_6_user_turn_on);
    context_6.add_context(cond_6_0);

    Proposition prop_6_body_0(3);
    BodyInstruction inst_0_6(BodyType::BELIEF, prop_6_body_0, EventOperator::BELIEF_DELETION);
    body_6.add_instruction(inst_0_6);

    Proposition prop_6_body_1(5);
    BodyInstruction inst_1_6(BodyType::BELIEF, prop_6_body_1, EventOperator::BELIEF_ADDITION);
    body_6.add_instruction(inst_1_6);

    Plan plan_6(EventOperator::BELIEF_ADDITION, prop_6, &context_6, &body_6);
    plan_base.add_plan(plan_6);

    //--------------------------------------------------------------------------

    Proposition prop_7(10);
    context_7 = Context(1);
    body_7 = Body(3);

    Proposition prop_7_eco_mode_available(3);
    ContextCondition cond_7_0(prop_7_eco_mode_available);
    context_7.add_context(cond_7_0);

    Proposition prop_7_body_0(11);
    BodyInstruction inst_0_7(BodyType::ACTION, prop_7_body_0, action_turn_off_lights);
    body_7.add_instruction(inst_0_7);

    Proposition prop_7_body_1(12);
    BodyInstruction inst_1_7(BodyType::ACTION, prop_7_body_1, action_raise_blinds);
    body_7.add_instruction(inst_1_7);

    Proposition prop_7_body_2(3);
    BodyInstruction inst_2_7(BodyType::BELIEF, prop_7_body_2, EventOperator::BELIEF_DELETION);
    body_7.add_instruction(inst_2_7);

    Plan plan_7(EventOperator::GOAL_ADDITION, prop_7, &context_7, &body_7);
    plan_base.add_plan(plan_7);

    //--------------------------------------------------------------------------

    Proposition prop_8(10);
    context_8 = Context(1);
    body_8 = Body(2);

    Proposition prop_8_standard_mode_available(5);
    ContextCondition cond_8_0(prop_8_standard_mode_available);
    context_8.add_context(cond_8_0);

    Proposition prop_8_body_0(13);
    BodyInstruction inst_0_8(BodyType::ACTION, prop_8_body_0, action_turn_on_lights);
    body_8.add_instruction(inst_0_8);

    Proposition prop_8_body_1(5);
    BodyInstruction inst_1_8(BodyType::BELIEF, prop_8_body_1, EventOperator::BELIEF_DELETION);
    body_8.add_instruction(inst_1_8);

    Plan plan_8(EventOperator::GOAL_ADDITION, prop_8, &context_8, &body_8);
    plan_base.add_plan(plan_8);

    //--------------------------------------------------------------------------

    Proposition prop_9(7);
    context_9 = Context(0);
    body_9 = Body(1);

    Proposition prop_9_body_0(14);
    BodyInstruction inst_0_9(BodyType::GOAL, prop_9_body_0, EventOperator::GOAL_ACHIEVE);
    body_9.add_instruction(inst_0_9);

    Plan plan_9(EventOperator::BELIEF_ADDITION, prop_9, &context_9, &body_9);
    plan_base.add_plan(plan_9);

    //--------------------------------------------------------------------------

    Proposition prop_10(14);
    context_10 = Context(2);
    body_10 = Body(2);

    Proposition prop_10_night_outside(4);
    ContextCondition cond_10_0(prop_10_night_outside);
    context_10.add_context(cond_10_0);

    Proposition prop_10_user_turn_off(7);
    ContextCondition cond_10_1(prop_10_user_turn_off);
    context_10.add_context(cond_10_1);

    Proposition prop_10_body_0(15);
    BodyInstruction inst_0_10(BodyType::ACTION, prop_10_body_0, action_lower_blinds);
    body_10.add_instruction(inst_0_10);

    Proposition prop_10_body_1(14);
    BodyInstruction inst_1_10(BodyType::GOAL, prop_10_body_1, EventOperator::GOAL_ACHIEVE);
    body_10.add_instruction(inst_1_10);

    Plan plan_10(EventOperator::GOAL_ADDITION, prop_10, &context_10, &body_10);
    plan_base.add_plan(plan_10);

    //--------------------------------------------------------------------------

    Proposition prop_11(14);
    context_11 = Context(2);
    body_11 = Body(3);

    Proposition prop_11_bright_inside(0);
    ContextCondition cond_11_0(prop_11_bright_inside);
    context_11.add_context(cond_11_0);

    Proposition prop_11_user_turn_off(7);
    ContextCondition cond_11_1(prop_11_user_turn_off);
    context_11.add_context(cond_11_1);

    Proposition prop_11_body_0(11);
    BodyInstruction inst_0_11(BodyType::ACTION, prop_11_body_0, action_turn_off_lights);
    body_11.add_instruction(inst_0_11);

    Proposition prop_11_body_1(15);
    BodyInstruction inst_1_11(BodyType::ACTION, prop_11_body_1, action_lower_blinds);
    body_11.add_instruction(inst_1_11);

    Proposition prop_11_body_2(14);
    BodyInstruction inst_2_11(BodyType::GOAL, prop_11_body_2, EventOperator::GOAL_ACHIEVE);
    body_11.add_instruction(inst_2_11);

    Plan plan_11(EventOperator::GOAL_ADDITION, prop_11, &context_11, &body_11);
    plan_base.add_plan(plan_11);

    //--------------------------------------------------------------------------

    Proposition prop_12(14);
    context_12 = Context(1);
    body_12 = Body(1);

    Proposition prop_12_user_turn_off(7);
    ContextCondition cond_12_0(prop_12_user_turn_off);
    context_12.add_context(cond_12_0);

    Proposition prop_12_body_0(14);
    BodyInstruction inst_0_12(BodyType::GOAL, prop_12_body_0, EventOperator::GOAL_ACHIEVE);
    body_12.add_instruction(inst_0_12);

    Plan plan_12(EventOperator::GOAL_DELETION, prop_12, &context_12, &body_12);
    plan_base.add_plan(plan_12);
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