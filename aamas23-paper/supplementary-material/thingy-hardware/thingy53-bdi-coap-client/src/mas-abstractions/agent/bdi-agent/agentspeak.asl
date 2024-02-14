!init.

+!init : agent_on <- !!maximize_agent_reward.

+!init <- !!init.

+!maximize_agent_reward : optimal_state <- !!maximize_agent_reward.

+!maximize_agent_reward : not_optimal_state & role_assigned <- leave_role; -role_assigned; !!maximize_agent_reward.

+!maximize_agent_reward : not_optimal_state & role_deleted <- enter_new_role; -role_deleted; !!maximize_agent_reward.

-!maximize_agent_reward <- !!maximize_agent_reward.

+!evaluate_costs : org_spec_changed & evaluation_in_progress <- get_org_spec_costs; -evaluation_in_progress.

+!evaluate_costs : rewards_changed & evaluation_in_progress <- get_rewards; -evaluation_in_progress.

+!evaluate_costs : local_capabilities_changed & evaluation_in_progress <- get_local_costs; -evaluation_in_progress.

+local_capabilities_changed <- +evaluation_in_progress; !!evaluate_costs.

+!reevaluate<- !evaluate_benefit; !decide.

+roles_available: have_spare_energy <- !!reevaluate.

+energy_state_changed : roles_available <- !!reevaluate.

+gossip_new_roles_available : have_spare_energy <- +roles_available.

+joined_role <- !commit_energy.

+!commit_energy : have_spare_energy
    <- !udpate_energy_state.

+!commit_energy
    <- !exit_role.

+org_spec_changed <- +evaluation_in_progress; !!evaluate_costs.

+rewards_changed <- +evaluation_in_progress; !!evaluate_costs.

+not_optimal_state <- -optimal_state; !!maximize_agent_reward.

-not_optimal_state <- +optimal_state; !!maximize_agent_reward.

+role_deleted <- !!maximize_agent_reward.