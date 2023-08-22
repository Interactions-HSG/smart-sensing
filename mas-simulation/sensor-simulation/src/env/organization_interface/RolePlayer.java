package organization_interface;

import com.google.gson.Gson;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.server.resources.CoapExchange;

public class RolePlayer extends CoapResource {

    Gson gson = new Gson();

    public PlayerInfo currentState;
    public RolePlayer(PlayerInfo info) {
        // set resource identifier
        super(info.id);
        currentState = info;
        setObservable(true);
        // set display name
        getAttributes().setTitle(String.format("Player:%s" , info.id));
    }

    @Override
    public void handleGET(CoapExchange exchange) {
        System.out.println("RolePlayer::handleGet");
       String data = gson.toJson(currentState);
       System.out.println(("->" + data));
       exchange.respond(data);
    }

    @Override
    public void handleDELETE(CoapExchange exchange) {
        GroupRole groupRole = (GroupRole)this.getParent();
        groupRole.removeRolePlayer(this);
        exchange.respond(CoAP.ResponseCode.DELETED);
    }
    @Override
    public void handlePUT(CoapExchange exchange) {
        String data = exchange.getRequestText();
        System.out.println("Received update:" + data);
        PlayerInfo newState = gson.fromJson(data, PlayerInfo.class);
        GroupRole groupRole = (GroupRole)this.getParent();
        groupRole.updateInfo(currentState,newState);
        currentState = newState;
        // respond to the request
        exchange.respond(CoAP.ResponseCode.CHANGED);
        changed();
    }

    public static class PlayerInfo {
        public String id;
        public int taskAllocation;
        public int canAllocateUpto;
        public double reward;
        public int cost;
        public int networkCost;
    }
}