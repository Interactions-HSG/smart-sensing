/*
 * AgentSpeak code:
 *
 * !init.
 * 
 * +!init : agent_on <- !!maximize_agent_reward.
 * 
 * +!init <- !!init.
 * 
 * +!maximize_agent_reward : optimal_state <- !!maximize_agent_reward.
 * 
 * +!maximize_agent_reward : not_optimal_state & role_assigned <- leave_role; -role_assigned; !!maximize_agent_reward.
 * 
 * +!maximize_agent_reward : not_optimal_state & role_deleted <- enter_new_role; -role_deleted; !!maximize_agent_reward.
 * 
 * -!maximize_agent_reward <- !!maximize_agent_reward.
 * 
 * +!evaluate_costs : org_spec_changed & evaluation_in_progress <- get_org_spec_costs; -evaluation_in_progress.
 * 
 * +!evaluate_costs : rewards_changed & evaluation_in_progress <- get_rewards; -evaluation_in_progress.
 * 
 * +!evaluate_costs : local_capabilities_changed & evaluation_in_progress <- get_local_costs; -evaluation_in_progress.
 * 
 * +local_capabilities_changed <- +evaluation_in_progress; !!evaluate_costs.
 * 
 * +org_spec_changed <- +evaluation_in_progress; !!evaluate_costs.
 * 
 * +rewards_changed <- +evaluation_in_progress; !!evaluate_costs.
 * 
 * +not_optimal_state <- -optimal_state; !!maximize_agent_reward.
 * 
 * -not_optimal_state <- +optimal_state; !!maximize_agent_reward.
 * 
 * +role_deleted <- !!maximize_agent_reward.
 */ 

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "bdi/belief_base.h"
#include "bdi/event_base.h"
#include "bdi/plan_base.h"
#include "bdi/intention_base.h"
#include "../../src/bdi-agent/functions.h"

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
  Body body_13;
  Context context_13;
  Body body_14;
  Context context_14;
  BeliefBase belief_base;
  EventBase event_base;
  PlanBase plan_base;
  IntentionBase intention_base;

public:
  AgentSettings()
  {
    belief_base = BeliefBase(9);
    event_base = EventBase(6);
    plan_base = PlanBase(15);
    intention_base = IntentionBase(10, 4);

    //--------------------------------------------------------------------------

    Belief belief_agent_on(0, update_agent_on, false);
    belief_base.add_belief(belief_agent_on);

    //--------------------------------------------------------------------------

    Belief belief_evaluation_in_progress(1, nullptr, false);
    belief_base.add_belief(belief_evaluation_in_progress);

    //--------------------------------------------------------------------------

    Belief belief_local_capabilities_changed(2, update_local_capabilities_changed, false);
    belief_base.add_belief(belief_local_capabilities_changed);

    //--------------------------------------------------------------------------

    Belief belief_not_optimal_state(3, update_not_optimal_state, false);
    belief_base.add_belief(belief_not_optimal_state);

    //--------------------------------------------------------------------------

    Belief belief_optimal_state(4, nullptr, false);
    belief_base.add_belief(belief_optimal_state);

    //--------------------------------------------------------------------------

    Belief belief_org_spec_changed(5, update_org_spec_changed, false);
    belief_base.add_belief(belief_org_spec_changed);

    //--------------------------------------------------------------------------

    Belief belief_rewards_changed(6, update_rewards_changed, false);
    belief_base.add_belief(belief_rewards_changed);

    //--------------------------------------------------------------------------

    Belief belief_role_assigned(7, update_role_assigned, false);
    belief_base.add_belief(belief_role_assigned);

    //--------------------------------------------------------------------------

    Belief belief_role_deleted(8, update_role_deleted, false);
    belief_base.add_belief(belief_role_deleted);

    //--------------------------------------------------------------------------

    Event event_9(EventOperator::GOAL_ADDITION, 9);
    event_base.add_event(event_9);

    //--------------------------------------------------------------------------

    Proposition prop_0(9);
    context_0 = Context(1);
    body_0 = Body(1);

    Proposition prop_0_agent_on(0);
    ContextCondition cond_0_0(prop_0_agent_on);
    context_0.add_context(cond_0_0);

    Proposition prop_0_body_0(10);
    BodyInstruction inst_0_0(BodyType::GOAL, prop_0_body_0, EventOperator::GOAL_ACHIEVE);
    body_0.add_instruction(inst_0_0);

    Plan plan_0(EventOperator::GOAL_ADDITION, prop_0, &context_0, &body_0);
    plan_base.add_plan(plan_0);

    //--------------------------------------------------------------------------

    Proposition prop_1(9);
    context_1 = Context(0);
    body_1 = Body(1);

    Proposition prop_1_body_0(9);
    BodyInstruction inst_0_1(BodyType::GOAL, prop_1_body_0, EventOperator::GOAL_ACHIEVE);
    body_1.add_instruction(inst_0_1);

    Plan plan_1(EventOperator::GOAL_ADDITION, prop_1, &context_1, &body_1);
    plan_base.add_plan(plan_1);

    //--------------------------------------------------------------------------

    Proposition prop_2(10);
    context_2 = Context(1);
    body_2 = Body(1);

    Proposition prop_2_optimal_state(4);
    ContextCondition cond_2_0(prop_2_optimal_state);
    context_2.add_context(cond_2_0);

    Proposition prop_2_body_0(10);
    BodyInstruction inst_0_2(BodyType::GOAL, prop_2_body_0, EventOperator::GOAL_ACHIEVE);
    body_2.add_instruction(inst_0_2);

    Plan plan_2(EventOperator::GOAL_ADDITION, prop_2, &context_2, &body_2);
    plan_base.add_plan(plan_2);

    //--------------------------------------------------------------------------

    Proposition prop_3(10);
    context_3 = Context(2);
    body_3 = Body(3);

    Proposition prop_3_not_optimal_state(3);
    ContextCondition cond_3_0(prop_3_not_optimal_state);
    context_3.add_context(cond_3_0);

    Proposition prop_3_role_assigned(7);
    ContextCondition cond_3_1(prop_3_role_assigned);
    context_3.add_context(cond_3_1);

    Proposition prop_3_body_0(11);
    BodyInstruction inst_0_3(BodyType::ACTION, prop_3_body_0, action_leave_role);
    body_3.add_instruction(inst_0_3);

    Proposition prop_3_body_1(7);
    BodyInstruction inst_1_3(BodyType::BELIEF, prop_3_body_1, EventOperator::BELIEF_DELETION);
    body_3.add_instruction(inst_1_3);

    Proposition prop_3_body_2(10);
    BodyInstruction inst_2_3(BodyType::GOAL, prop_3_body_2, EventOperator::GOAL_ACHIEVE);
    body_3.add_instruction(inst_2_3);

    Plan plan_3(EventOperator::GOAL_ADDITION, prop_3, &context_3, &body_3);
    plan_base.add_plan(plan_3);

    //--------------------------------------------------------------------------

    Proposition prop_4(10);
    context_4 = Context(2);
    body_4 = Body(3);

    Proposition prop_4_not_optimal_state(3);
    ContextCondition cond_4_0(prop_4_not_optimal_state);
    context_4.add_context(cond_4_0);

    Proposition prop_4_role_deleted(8);
    ContextCondition cond_4_1(prop_4_role_deleted);
    context_4.add_context(cond_4_1);

    Proposition prop_4_body_0(12);
    BodyInstruction inst_0_4(BodyType::ACTION, prop_4_body_0, action_enter_new_role);
    body_4.add_instruction(inst_0_4);

    Proposition prop_4_body_1(8);
    BodyInstruction inst_1_4(BodyType::BELIEF, prop_4_body_1, EventOperator::BELIEF_DELETION);
    body_4.add_instruction(inst_1_4);

    Proposition prop_4_body_2(10);
    BodyInstruction inst_2_4(BodyType::GOAL, prop_4_body_2, EventOperator::GOAL_ACHIEVE);
    body_4.add_instruction(inst_2_4);

    Plan plan_4(EventOperator::GOAL_ADDITION, prop_4, &context_4, &body_4);
    plan_base.add_plan(plan_4);

    //--------------------------------------------------------------------------

    Proposition prop_5(10);
    context_5 = Context(0);
    body_5 = Body(1);

    Proposition prop_5_body_0(10);
    BodyInstruction inst_0_5(BodyType::GOAL, prop_5_body_0, EventOperator::GOAL_ACHIEVE);
    body_5.add_instruction(inst_0_5);

    Plan plan_5(EventOperator::GOAL_DELETION, prop_5, &context_5, &body_5);
    plan_base.add_plan(plan_5);

    //--------------------------------------------------------------------------

    Proposition prop_6(13);
    context_6 = Context(2);
    body_6 = Body(2);

    Proposition prop_6_org_spec_changed(5);
    ContextCondition cond_6_0(prop_6_org_spec_changed);
    context_6.add_context(cond_6_0);

    Proposition prop_6_evaluation_in_progress(1);
    ContextCondition cond_6_1(prop_6_evaluation_in_progress);
    context_6.add_context(cond_6_1);

    Proposition prop_6_body_0(14);
    BodyInstruction inst_0_6(BodyType::ACTION, prop_6_body_0, action_get_org_spec_costs);
    body_6.add_instruction(inst_0_6);

    Proposition prop_6_body_1(1);
    BodyInstruction inst_1_6(BodyType::BELIEF, prop_6_body_1, EventOperator::BELIEF_DELETION);
    body_6.add_instruction(inst_1_6);

    Plan plan_6(EventOperator::GOAL_ADDITION, prop_6, &context_6, &body_6);
    plan_base.add_plan(plan_6);

    //--------------------------------------------------------------------------

    Proposition prop_7(13);
    context_7 = Context(2);
    body_7 = Body(2);

    Proposition prop_7_rewards_changed(6);
    ContextCondition cond_7_0(prop_7_rewards_changed);
    context_7.add_context(cond_7_0);

    Proposition prop_7_evaluation_in_progress(1);
    ContextCondition cond_7_1(prop_7_evaluation_in_progress);
    context_7.add_context(cond_7_1);

    Proposition prop_7_body_0(15);
    BodyInstruction inst_0_7(BodyType::ACTION, prop_7_body_0, action_get_rewards);
    body_7.add_instruction(inst_0_7);

    Proposition prop_7_body_1(1);
    BodyInstruction inst_1_7(BodyType::BELIEF, prop_7_body_1, EventOperator::BELIEF_DELETION);
    body_7.add_instruction(inst_1_7);

    Plan plan_7(EventOperator::GOAL_ADDITION, prop_7, &context_7, &body_7);
    plan_base.add_plan(plan_7);

    //--------------------------------------------------------------------------

    Proposition prop_8(13);
    context_8 = Context(2);
    body_8 = Body(2);

    Proposition prop_8_local_capabilities_changed(2);
    ContextCondition cond_8_0(prop_8_local_capabilities_changed);
    context_8.add_context(cond_8_0);

    Proposition prop_8_evaluation_in_progress(1);
    ContextCondition cond_8_1(prop_8_evaluation_in_progress);
    context_8.add_context(cond_8_1);

    Proposition prop_8_body_0(16);
    BodyInstruction inst_0_8(BodyType::ACTION, prop_8_body_0, action_get_local_costs);
    body_8.add_instruction(inst_0_8);

    Proposition prop_8_body_1(1);
    BodyInstruction inst_1_8(BodyType::BELIEF, prop_8_body_1, EventOperator::BELIEF_DELETION);
    body_8.add_instruction(inst_1_8);

    Plan plan_8(EventOperator::GOAL_ADDITION, prop_8, &context_8, &body_8);
    plan_base.add_plan(plan_8);

    //--------------------------------------------------------------------------

    Proposition prop_9(2);
    context_9 = Context(0);
    body_9 = Body(2);

    Proposition prop_9_body_0(1);
    BodyInstruction inst_0_9(BodyType::BELIEF, prop_9_body_0, EventOperator::BELIEF_ADDITION);
    body_9.add_instruction(inst_0_9);

    Proposition prop_9_body_1(13);
    BodyInstruction inst_1_9(BodyType::GOAL, prop_9_body_1, EventOperator::GOAL_ACHIEVE);
    body_9.add_instruction(inst_1_9);

    Plan plan_9(EventOperator::BELIEF_ADDITION, prop_9, &context_9, &body_9);
    plan_base.add_plan(plan_9);

    //--------------------------------------------------------------------------

    Proposition prop_10(5);
    context_10 = Context(0);
    body_10 = Body(2);

    Proposition prop_10_body_0(1);
    BodyInstruction inst_0_10(BodyType::BELIEF, prop_10_body_0, EventOperator::BELIEF_ADDITION);
    body_10.add_instruction(inst_0_10);

    Proposition prop_10_body_1(13);
    BodyInstruction inst_1_10(BodyType::GOAL, prop_10_body_1, EventOperator::GOAL_ACHIEVE);
    body_10.add_instruction(inst_1_10);

    Plan plan_10(EventOperator::BELIEF_ADDITION, prop_10, &context_10, &body_10);
    plan_base.add_plan(plan_10);

    //--------------------------------------------------------------------------

    Proposition prop_11(6);
    context_11 = Context(0);
    body_11 = Body(2);

    Proposition prop_11_body_0(1);
    BodyInstruction inst_0_11(BodyType::BELIEF, prop_11_body_0, EventOperator::BELIEF_ADDITION);
    body_11.add_instruction(inst_0_11);

    Proposition prop_11_body_1(13);
    BodyInstruction inst_1_11(BodyType::GOAL, prop_11_body_1, EventOperator::GOAL_ACHIEVE);
    body_11.add_instruction(inst_1_11);

    Plan plan_11(EventOperator::BELIEF_ADDITION, prop_11, &context_11, &body_11);
    plan_base.add_plan(plan_11);

    //--------------------------------------------------------------------------

    Proposition prop_12(3);
    context_12 = Context(0);
    body_12 = Body(2);

    Proposition prop_12_body_0(4);
    BodyInstruction inst_0_12(BodyType::BELIEF, prop_12_body_0, EventOperator::BELIEF_DELETION);
    body_12.add_instruction(inst_0_12);

    Proposition prop_12_body_1(10);
    BodyInstruction inst_1_12(BodyType::GOAL, prop_12_body_1, EventOperator::GOAL_ACHIEVE);
    body_12.add_instruction(inst_1_12);

    Plan plan_12(EventOperator::BELIEF_ADDITION, prop_12, &context_12, &body_12);
    plan_base.add_plan(plan_12);

    //--------------------------------------------------------------------------

    Proposition prop_13(3);
    context_13 = Context(0);
    body_13 = Body(2);

    Proposition prop_13_body_0(4);
    BodyInstruction inst_0_13(BodyType::BELIEF, prop_13_body_0, EventOperator::BELIEF_ADDITION);
    body_13.add_instruction(inst_0_13);

    Proposition prop_13_body_1(10);
    BodyInstruction inst_1_13(BodyType::GOAL, prop_13_body_1, EventOperator::GOAL_ACHIEVE);
    body_13.add_instruction(inst_1_13);

    Plan plan_13(EventOperator::BELIEF_DELETION, prop_13, &context_13, &body_13);
    plan_base.add_plan(plan_13);

    //--------------------------------------------------------------------------

    Proposition prop_14(8);
    context_14 = Context(0);
    body_14 = Body(1);

    Proposition prop_14_body_0(10);
    BodyInstruction inst_0_14(BodyType::GOAL, prop_14_body_0, EventOperator::GOAL_ACHIEVE);
    body_14.add_instruction(inst_0_14);

    Plan plan_14(EventOperator::BELIEF_ADDITION, prop_14, &context_14, &body_14);
    plan_base.add_plan(plan_14);
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