#include "mas-abstractions/organization/Organization.h"

class ReactiveAgent
{
    public:
        void run();
    private:
        double getEnergyInBuffer();
        double getEnergyCommitted();
        void delibrate();
        double calculateBenefit(GroupRoleInfo* group, double buffer, double committed);
        double calculateCost(GroupRoleInfo* group, double buffer, double committed);
};