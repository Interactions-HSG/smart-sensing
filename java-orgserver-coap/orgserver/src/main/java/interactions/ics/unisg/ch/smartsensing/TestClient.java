package interactions.ics.unisg.ch.smartsensing;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import com.google.gson.Gson;
import interactions.ics.unisg.ch.smartsensing.entities.GroupRole;
import interactions.ics.unisg.ch.smartsensing.entities.RolePlayer;
import org.eclipse.californium.core.CoapClient;
import org.eclipse.californium.core.CoapHandler;
import org.eclipse.californium.core.CoapObserveRelation;
import org.eclipse.californium.core.CoapResponse;
import org.eclipse.californium.core.coap.MediaTypeRegistry;
import org.eclipse.californium.elements.exception.ConnectorException;

public class TestClient {

    public static void testConnection(String[] args){

        CoapClient client = new CoapClient("coap://localhost:5683/room1");

        System.out.println("SYNCHRONOUS");
        Gson gson = new Gson();
        // synchronous
        String response = null;
        try {

            //Query role player
            //client.setURI("coap://10.0.1.8:5683/room1/gr_comfort_sensing/ag1");
            //response = client.get().getResponseText();
            //System.out.println("Get ag1: " + response);

            response = client.get().getResponseText();
            System.out.println("Get group info: " + response);

            //Create a new GroupRole
            GroupRole.GroupRoleInfo grinfo = new GroupRole.GroupRoleInfo();
            GroupRole.FunctionalSpec fspec = new GroupRole.FunctionalSpec();
            fspec.hasQuantityKind = 0;
            fspec.measurementInterval = 30000;
            fspec.updateInterval = 60000;
            fspec.measurementDuration = 60; //in minutes
            grinfo.id = "gr_comfort_sensing";
            grinfo.maxAgents = 2;
            grinfo.minAllocation = 50;
            grinfo.reward = 3;
            grinfo.functionalSpecification = fspec;
            response = client.post(gson.toJson(grinfo), MediaTypeRegistry.APPLICATION_JSON).getResponseText();
            System.out.println("Create new group role: " + response);

            //Create a new GroupRole
            GroupRole.GroupRoleInfo grinfo2 = new GroupRole.GroupRoleInfo();
            GroupRole.FunctionalSpec fspec2 = new GroupRole.FunctionalSpec();
            fspec2.hasQuantityKind = 0;
            fspec2.measurementInterval = 60000;
            fspec2.updateInterval = 60000;
            fspec2.measurementDuration = 60;
            grinfo2.id = "gr_safety_sensing";
            grinfo2.maxAgents = 2;
            grinfo2.minAllocation = 50;
            grinfo2.reward = 2;
            grinfo2.functionalSpecification =fspec2;
            response = client.post(gson.toJson(grinfo2), MediaTypeRegistry.APPLICATION_JSON).getResponseText();
            System.out.println("Create new group role: " + response);

            response = client.get().getResponseText();
            System.out.println("Get group info: " + response);

            client.setURI("coap://localhost:5683/room1/gr_comfort_sensing");
            response = client.get().getResponseText();
            System.out.println("Get gr_comfort_sensing: " + response);

            //Add a role player
            /*
            RolePlayer.PlayerInfo pi = new RolePlayer.PlayerInfo();
            pi.id = "ag1";
            pi.taskAllocation = 50;
            response = client.post(gson.toJson(pi), MediaTypeRegistry.APPLICATION_JSON).getCode().toString();
            System.out.println("Create new agent in  gr_comfort_sensing: " + response);

            response = client.get().getResponseText();
            System.out.println("Get gr_comfort_sensing: " + response);
*/
            //Query role player
            //client.setURI("coap://localhost:5683/room1/gr_comfort_sensing/ag1");
            //response = client.get().getResponseText();
            //System.out.println("Get ag1: " + response);

            //Delete agent as role player
            //response = client.delete().getCode().toString();
            //System.out.println("Delete agent in  gr_comfort_sensing: " + response);

            //Get updated info
            //client.setURI("coap://localhost:5683/room1/gr_comfort_sensing");
            //response = client.get().getResponseText();
            //System.out.println("Get gr_comfort_sensing: " + response);

            //Delete group role
            //client.setURI("coap://localhost:5683/room1/gr_comfort_sensing");
            //response = client.delete().getResponseText();
            //System.out.println("Delete gr_comfort_sensing: " + response);

        } catch (ConnectorException e) {
            throw new RuntimeException(e);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        //CoapResponse resp2 = client.post("payload", MediaTypeRegistry.TEXT_PLAIN);
        //System.out.println("RESPONSE 2 CODE: " + resp2.getCode());

        // asynchronous

//        System.out.println("ASYNCHRONOUS (press enter to continue)");
//
//        client.get(new CoapHandler() {
//            @Override public void onLoad(CoapResponse response) {
//                String content = response.getResponseText();
//                System.out.println("RESPONSE 3: " + content);
//            }
//
//            @Override public void onError() {
//                System.err.println("FAILED");
//            }
//        });
//
//        // wait for user
//        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
//        try { br.readLine(); } catch (IOException e) { }
//
//        // observe
//
//        System.out.println("OBSERVE (press enter to exit)");
//
//        CoapObserveRelation relation = client.observe(
//                new CoapHandler() {
//                    @Override public void onLoad(CoapResponse response) {
//                        String content = response.getResponseText();
//                        System.out.println("NOTIFICATION: " + content);
//                    }
//
//                    @Override public void onError() {
//                        System.err.println("OBSERVING FAILED (press enter to exit)");
//                    }
//                });
//
//        // wait for user
//        try { br.readLine(); } catch (IOException e) { }
//
//        System.out.println("CANCELLATION");
//
//        relation.proactiveCancel();
    }
}
