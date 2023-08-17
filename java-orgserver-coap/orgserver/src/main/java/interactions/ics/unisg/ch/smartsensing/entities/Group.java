package interactions.ics.unisg.ch.smartsensing.entities;

import com.google.gson.Gson;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.eclipse.californium.core.server.resources.Resource;

import java.util.ArrayList;
import java.util.List;

public class Group extends CoapResource {
    Gson gson = new Gson();

    public Group(String id) {
        // set resource identifier
        super(id);
        setObservable(true);
        // set display name
        getAttributes().setTitle(String.format("Group:%s" , id));
    }

    @Override
    public void handleGET(CoapExchange exchange) {
        // respond to the request
        try {
            List<GroupRole.GroupRoleInfo> grs = new ArrayList<>();
            for(Resource res : this.getChildren()) {
                GroupRole gr = (GroupRole)res;
                grs.add(gr.specification);
            }
            GroupRole.GroupRoleInfos col = new GroupRole.GroupRoleInfos();
            col.elements = grs;
            col.num_elements = grs.size();
            String response = gson.toJson(col);
            exchange.respond(response);
        }catch (Exception e){
            System.out.println(e.getMessage());
        }
    }

    @Override
    public void handlePOST(CoapExchange exchange) {
        String data = exchange.getRequestText();
        // respond to the request
        GroupRole.GroupRoleInfo spec = gson.fromJson(data, GroupRole.GroupRoleInfo.class);
        addGroupRole(spec);
        exchange.respond(CoAP.ResponseCode.CREATED);
        changed();
    }

    private void addGroupRole(GroupRole.GroupRoleInfo spec){
        GroupRole groupRole = new GroupRole(spec);
        this.add(groupRole);
    }
}
