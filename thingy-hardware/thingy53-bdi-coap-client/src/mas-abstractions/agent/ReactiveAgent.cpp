#include "mas-abstractions/agent/ReactiveAgent.h"
#include "peripherals/battery.h"

#define MIN_ENERGY 5.0

void ReactiveAgent::run(){
    Organization::refresh();
}

double ReactiveAgent::getEnergyInBuffer(){
    return (battery_sample() - 3000) / 1200.0f;
}

double ReactiveAgent::getEnergyCommitted(){
    std::list<PlayerInfo*> enacting = Organization::getMyRoles();
    double commited = 0.0f;
    for(PlayerInfo* r : enacting){
        commited += r->cost;
    }
    return commited;
}

double ReactiveAgent::calculateCost(GroupRoleInfo* group, double buffer, double committed){
    double e_m = group->functionalSpecification.measurementDuration * 0.15;
    double e_a = buffer - committed;
    double cost = e_m / e_a;
    return cost;
}

double ReactiveAgent::calculateBenefit(GroupRoleInfo* group, double buffer, double committed){
    double reward = group->reward;
    double cost = calculateCost(group, buffer, committed);
    double benefit = reward - cost;
    return 0.0;
}

void ReactiveAgent::delibrate(){
    //Sum up energy required for current roles
    double e_c = getEnergyCommitted();
    double e_b = getEnergyInBuffer();

    //Spare energy? Get available roles
    if(e_b - e_c > MIN_ENERGY) //Considering 5J as minimum usable
    {
        std::list<GroupRoleInfo*> availableRoles = Organization::getAvailableRoles();
        for(GroupRoleInfo* gr : availableRoles)
        {
            if(calculateBenefit(gr, e_b, e_c) > 0){
                Organization::joinRole(gr->id);
                break; //TODO: We can join multiple roles, but keeping it simple..for now.
            }
        }
    }
}