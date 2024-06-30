package interactions.ics.unisg.ch.smartsensing.entities;

import com.google.gson.Gson;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.eclipse.californium.core.server.resources.Resource;

import java.util.ArrayList;
import java.util.List;

public class GroupRole extends CoapResource {
    Gson gson = new Gson();

    public GroupRoleInfo specification;
    MeasurementReceiver receiver;
    public GroupRole(GroupRoleInfo spec) {
        // set resource identifier
        super(spec.id);
        specification = spec;
        setObservable(true);
        // set display name
        getAttributes().setTitle(String.format("GroupRole:%s" , spec.id));
        receiver = new MeasurementReceiver("receiver");
        this.add(receiver);
    }

    @Override
    public void handleGET(CoapExchange exchange) {
        // respond to the request
        System.out.printf("GroupRole:Get request for %s from %s\n", this.getName(), exchange.getSourceAddress());
        if(exchange.getQueryParameter("players") != null){
            List<PlayerInfo> players = new ArrayList<>();
            for(Resource r : this.getChildren()){
                RolePlayer rp = (RolePlayer)r;
                players.add(rp.playerInfo);
            }
            String data = gson.toJson(players);
            exchange.respond(data);
            System.out.printf("Group:Get request for %s responded %s\n", this.getName(), data);
        }else {
            specification.currentAgents = this.getChildren().size();
            String data = gson.toJson(specification);
            exchange.respond(data);
        }
    }

    @Override
    public void handlePATCH(CoapExchange exchange){
        String data = exchange.getRequestText();
        //System.out.println("Received update:" + data);
        GroupRoleInfo groupRoleInfo = gson.fromJson(data, GroupRoleInfo.class);

        exchange.respond(CoAP.ResponseCode.CHANGED);
        //changed();
    }

    @Override
    public void handlePUT(CoapExchange exchange) {
        String data = exchange.getRequestText();
        System.out.printf("GroupRole:Update request for %s with data %s from %s\n", this.getName(), data,exchange.getSourceAddress());
        //System.out.println("Received update:" + data);
        specification = gson.fromJson(data, GroupRoleInfo.class);
        exchange.respond(CoAP.ResponseCode.CHANGED);
        //changed();
        receiver.reset();
        Group group = (Group) this.getParent();
        group.signalChange();
    }

    final Object lock = new Object();
    @Override
    public void handlePOST(CoapExchange exchange) {
        System.out.printf("GroupRole: Received POST for %s with data %s from %s\n", this.getName(), exchange.getRequestText(), exchange.getSourceAddress());
        synchronized (lock) {
            if (this.getChildren().size() >= specification.maxAgents || specification.currentAllocation >= 100) {
                exchange.respond(CoAP.ResponseCode.FORBIDDEN);
                return;
            }
            String resource = exchange.getRequestText();
            // respond to the request
            PlayerInfo playerState = gson.fromJson(resource, PlayerInfo.class);
            addRolePlayer(resource, playerState);
            exchange.respond(CoAP.ResponseCode.CREATED);
        }
        //changed();
        //Group group = (Group) this.getParent();
        //group.signalChange();
    }

    @Override
    public void handleDELETE(CoapExchange exchange) {
        System.out.printf("GroupRole:Delete request for %s from %s \n", this.getName(), exchange.getSourceAddress());
        if(this.getChildren().size() == 0){
            this.getParent().delete(this);
            exchange.respond(CoAP.ResponseCode.DELETED);
        }
        else{
            exchange.respond(CoAP.ResponseCode.FORBIDDEN);
        }
        //Group group = (Group) this.getParent();
        //group.signalChange();
    }

    private void addRolePlayer(String roleName, PlayerInfo state){
        RolePlayer player = new RolePlayer(state);
        player.playerInfo.reward = specification.reward * (double)player.playerInfo.taskAllocation/100.0f;
        specification.reward -= player.playerInfo.reward;
        specification.currentAllocation += player.playerInfo.taskAllocation;
        this.add(player);
        System.out.printf("GroupRole:Added RolePlayer %s in GroupRole %s\n", state.id, roleName);
    }

    protected void removeRolePlayer(RolePlayer player){
        this.specification.reward += player.playerInfo.reward;
        specification.currentAllocation -= player.playerInfo.taskAllocation;
        specification.currentAgents--;
        this.delete(player);
        System.out.printf("GroupRole:Removed RolePlayer %s in GroupRole %s\n", player.playerInfo.id, this.getName());
    }

    protected void updateInfo(PlayerInfo currentInfo, PlayerInfo newInfo){
        this.specification.reward += currentInfo.reward;
        specification.currentAllocation -= currentInfo.taskAllocation;
        this.specification.reward -= newInfo.reward;
        specification.currentAllocation += newInfo.taskAllocation;
        System.out.printf("GroupRole:Updated GroupRole %s\n", this.getName());
    }






}
