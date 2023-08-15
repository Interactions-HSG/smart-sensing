package interactions.ics.unisg.ch.smartsensing.entities;

import com.google.gson.Gson;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.server.resources.CoapExchange;

public class GroupRole extends CoapResource {
    Gson gson = new Gson();

    public GroupRoleInfo specification;
    public GroupRole(GroupRoleInfo spec) {
        // set resource identifier
        super(spec.id);
        specification = spec;
        setObservable(true);
        // set display name
        getAttributes().setTitle(String.format("GroupRole:%s" , spec.id));
    }

    @Override
    public void handleGET(CoapExchange exchange) {
        // respond to the request
        specification.currentAgents = this.getChildren().size();
        String data = gson.toJson(specification);
        exchange.respond(data);
    }

    @Override
    public void handlePUT(CoapExchange exchange) {
        String data = exchange.getRequestText();
        System.out.println("Received update:" + data);
        specification = gson.fromJson(data, GroupRoleInfo.class);
        exchange.respond(CoAP.ResponseCode.CHANGED);
        changed();
    }

    @Override
    public void handlePOST(CoapExchange exchange) {
        if(this.getChildren().size() >= specification.maxAgents){
            exchange.respond(CoAP.ResponseCode.FORBIDDEN);
            return;
        }
        String resource = exchange.getRequestText();
        // respond to the request
        RolePlayer.PlayerInfo playerState = gson.fromJson(resource, RolePlayer.PlayerInfo.class);
        addRolePlayer(resource, playerState);
        exchange.respond(CoAP.ResponseCode.CHANGED);
        changed();
    }

    @Override
    public void handleDELETE(CoapExchange exchange) {
        if(this.getChildren().size() == 0){
            this.getParent().delete(this);
            exchange.respond(CoAP.ResponseCode.DELETED);
        }
        else{
            exchange.respond(CoAP.ResponseCode.FORBIDDEN);
        }
    }

    private void addRolePlayer(String roleName, RolePlayer.PlayerInfo state){
        RolePlayer player = new RolePlayer(state);
        player.currentState.reward = specification.reward * player.currentState.taskAllocation/100;
        specification.reward -= player.currentState.reward;
        specification.currentAllocation += player.currentState.taskAllocation;
        this.add(player);
    }

    protected void removeRolePlayer(RolePlayer player){
        this.specification.reward += player.currentState.reward;
        specification.currentAllocation -= player.currentState.taskAllocation;
        this.delete(player);
    }

    protected void updateInfo(RolePlayer.PlayerInfo currentInfo, RolePlayer.PlayerInfo newInfo){
        this.specification.reward += currentInfo.reward;
        specification.currentAllocation -= currentInfo.taskAllocation;
        this.specification.reward -= newInfo.reward;
        specification.currentAllocation += newInfo.taskAllocation;
    }

    public static class FunctionalSpec {
        public int hasQuantityKind;
        public int measurementInterval;
        public int updateInterval;
    }

    public static class GroupRoleInfo {
        public String id;
        public Boolean isActive = true;
        public FunctionalSpec functionalSpecification;
        public int minAllocation = 100;

        public int currentAllocation = 0;
        public int currentAgents = 0;
        public int minAgents = 1;
        public int maxAgents = 1;
        public int reward = 0;
    }
}
