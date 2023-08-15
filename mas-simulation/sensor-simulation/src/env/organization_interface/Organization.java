package organization_interface;

import cartago.Artifact;
import cartago.OPERATION;
import cartago.ObsProperty;
import com.google.gson.Gson;
import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.elements.exception.ConnectorException;

import java.io.IOException;


//import org.eclipse.californium.core.CoapClient;

public class Organization {



    public static GroupRole.GroupRoleInfo[] getGroupRoles(){

        CoapClient client = new CoapClient("coap://10.0.1.7:5683/room1");

        System.out.println("SYNCHRONOUS");
        Gson gson = new Gson();
        // synchronous
        String response = null;
        try {
            response = client.get().getResponseText();
            System.out.println("Get group info: " + response);
            GroupRole.GroupRoleInfo[] roles = gson.fromJson(response, GroupRole.GroupRoleInfo[].class);
            return roles;

        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

    }
}
