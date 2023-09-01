package organization_interface;

public class PlayerInfo {
    public String id;
    public String groupRoleId;
    public int taskAllocation;
    public int canAllocateUpto;
    public double reward;
    public double cost;
    public double networkCost;
    public double benefit;
    public long startTime;

    public double getBenefit(){return benefit;}
}
