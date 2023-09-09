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
            List<GroupRoleInfo> grs = new ArrayList<>();
            for(Resource res : this.getChildren()) {
                GroupRole gr = (GroupRole)res;
                grs.add(gr.specification);
            }
            GroupRoleInfos col = new GroupRoleInfos();
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
        GroupRoleInfo spec = gson.fromJson(data, GroupRoleInfo.class);
        addGroupRole(spec);
        exchange.respond(CoAP.ResponseCode.CREATED);
        changed();
    }

    public void signalChange(){
        changed();
    }

    private void addGroupRole(GroupRoleInfo spec){
        GroupRole groupRole = new GroupRole(spec);
        this.add(groupRole);
        System.out.printf("New GroupRole %s created by %s\n", spec.id, spec.creatorId);
    }
}
