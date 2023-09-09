package organization_interface;

import java.util.ArrayList;
import java.util.List;

public class GroupRole {
    List<PlayerInfo> players = new ArrayList<>();

    GroupRoleInfo specification;
    double originalReward = 0;
    int measurementCount = 0;

    GroupRole(GroupRoleInfo spec){
        this.specification = spec;
        originalReward = specification.reward;
    }

    public boolean addPlayer(PlayerInfo player){
        if(this.specification.currentAllocation >= 100){
            return false;
        }
        players.add(player);
        player.reward =originalReward;// specification.reward * (double)player.taskAllocation/100.0f;
        specification.reward = 0;// -= player.reward;
        specification.currentAllocation = 100;// += player.taskAllocation;
        specification.currentAgents++;
        return true;
    }

    public void removePlayer(PlayerInfo player){
        this.specification.reward = originalReward; // += player.reward;
        specification.currentAllocation = 0;// -= player.taskAllocation;
        specification.currentAgents--;
        players.remove(player);
    }

    public GroupRoleInfo getSpecification(){
        return specification;
    }

    public void updateSpecification(GroupRoleInfo spec){
        if(spec.isActive && !specification.isActive){
            measurementCount = 0;
        }
        this.specification.isActive = spec.isActive;
        this.specification.isActiveSince = spec.isActiveSince;
        this.specification.reward = spec.reward;
        originalReward = specification.reward;
    }

    public void receiveMeasurement(String data){
        measurementCount++;
    }

    public int getMeasurementCount(){
        return measurementCount;
    }
}
