package interactions.ics.unisg.ch.smartsensing;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;

import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.CoapServer;
import org.eclipse.californium.core.coap.CoAP.ResponseCode;
import org.eclipse.californium.core.network.CoapEndpoint;
import org.eclipse.californium.core.server.resources.CoapExchange;
import org.eclipse.californium.elements.config.Configuration;
import org.eclipse.californium.elements.tcp.netty.TcpServerConnector;
import org.eclipse.californium.elements.util.NetworkInterfacesUtil;

import com.google.gson.Gson;

import moise.common.MoiseCardinalityException;
import moise.common.MoiseConsistencyException;
import moise.common.MoiseElement;
import moise.common.MoiseException;
import moise.oe.GoalInstance;
import moise.oe.GroupInstance;
import moise.oe.MissionPlayer;
import moise.oe.OE;
import moise.oe.OEAgent;
import moise.oe.Permission;
import moise.oe.RolePlayer;
import moise.oe.SchemeInstance;
import moise.os.OS;
import moise.os.fs.Goal;
import moise.os.fs.Scheme;
import moise.os.ss.Group;
import moise.os.ss.Role;

public class MoiseOrgServer extends CoapServer {
	
	static TreeNode orgspecnode = new TreeNode("org-spec");
	static TreeNode orgspecnodetemp = new TreeNode("org-spec-segmented");
	static TreeNode orgentitynode = new TreeNode("org-entities");	
	static TreeNode agents = new TreeNode("oe-agents");
	static PubSubResource oegroups = new PubSubResource("oe-groups", "groups");
	static TreeNode oeschemes = new TreeNode("oe-schemes");
	static TreeNode oegoals = new TreeNode("oe-goals");
	
	static TreeNode osfs = new TreeNode("os-fs");
	static TreeNode osgroups = new TreeNode("os-groups");
	static TreeNode osroles = new TreeNode("os-roles");
	static TreeNode osss = new TreeNode("os-ss");
	static TreeNode osschemes = new TreeNode("os-schemes");
	static TreeNode osns = new TreeNode("os-ns");

	static TreeNode measurements = new TreeNode("measurements");
	
	static OS orgSpec ;	
	static OE orgEntity;
	
	static Gson gson = new Gson();
	
	public MoiseOrgServer(boolean udp, boolean tcp, int port) throws SocketException {
		addEndpoints(udp, tcp, port);
		try {
			orgSpec = OS.loadOSFromURI(Launcher.fileName);
			orgEntity = new OE(new GoalInstance(new Goal("blah"), null), orgSpec);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		oegroups.setObservable(true);
		orgentitynode.add(agents);
		orgentitynode.add(oegroups);
		orgentitynode.add(oeschemes);
		orgentitynode.add(oegoals);
		add(orgentitynode);
		
		osfs.add(osgroups);
		osfs.add(osroles);	
		orgspecnode.add(osfs);
		osss.add(osschemes);
		orgspecnode.add(osss);
		orgspecnode.add(osns);
		add(orgspecnode);
		add(orgspecnodetemp);
		
		
		PubSubResource data = new PubSubResource("data");


		measurements.add(data);
		add(measurements);
		//add(new MyIpResource(MyIpResource.RESOURCE_NAME, true));
		addOSGroups();
		addOSRoles();
		addOSSchemes();
		addOEGroups();
		System.out.println("OrgServer is now ready. Browse to coap://<inteface>:" + (port));
	}
	
	/**
	 * Add individual endpoints listening on default CoAP port on all IPv4
	 * addresses of all network interfaces.
	 */
	private void addEndpoints(boolean udp, boolean tcp, int port) {
		Configuration config = Configuration.getStandard();
		for (InetAddress addr : NetworkInterfacesUtil.getNetworkInterfaces()) {
			InetSocketAddress bindToAddress = new InetSocketAddress(addr, port);
			if (udp) {
				CoapEndpoint.Builder builder = new CoapEndpoint.Builder();
				builder.setInetSocketAddress(bindToAddress);
				builder.setConfiguration(config);
				addEndpoint(builder.build());
			}
			if (tcp) {
				TcpServerConnector connector = new TcpServerConnector(bindToAddress, config);
				CoapEndpoint.Builder builder = new CoapEndpoint.Builder();
				builder.setConnector(connector);
				builder.setConfiguration(config);
				addEndpoint(builder.build());
			}
		}
	}
	
	private void addOSRoles() {
		for(Role role : orgSpec.getSS().getRolesDef()) {
			osroles.add(new OSResource(role));
		}
	}
	
	private void addOSGroups() {
		Group root = orgSpec.getSS().getRootGrSpec();
		OSResource rg = new OSResource(root);
		osgroups.add(rg);
	}
	
	private void addOSSchemes() {
		for(Scheme scheme: orgSpec.getFS().getSchemes())
		{
			osschemes.add(new OSResource(scheme));
		}
	}
	
	private void addOEGroups() {
		Group root = orgSpec.getSS().getRootGrSpec();
		GroupInstance gi;
		try {
			gi = orgEntity.addGroup(root.getId());
		
			OEResource rg = new OEResource(gi);
			oegroups.add(rg);
		} catch (MoiseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		for(Scheme scheme : orgSpec.getFS().getSchemes()) {
			
			try{				
				SchemeInstance si = orgEntity.startScheme(scheme.getId());
				oeschemes.add(new OEResource(si));
				//TODO:Assignment of schemes to groups should be done based on an input config
				for(GroupInstance g : orgEntity.getGroups()) {
					si.addResponsibleGroup(g);
				}
			} catch (MoiseException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

	}
	
	static class TreeNode extends CoapResource {
		public TreeNode(String id) {
			super(id);
			if(id.equals("org-entities")) {
				setObservable(true);
			}
		}
		
		private void handleOrgSpecResource(CoapExchange exchange) {
			if(!(this.getName().equals("org-spec") || this.getName().equals("org-spec-segmented"))) {
				exchange.respond(ResponseCode.METHOD_NOT_ALLOWED);
				return;
			}
			// respond to the request
			try {
				String content = Files.readString(Path.of(Launcher.fileName));
				byte[] data = content.getBytes();
				if(this.getName().equals("org-spec-segmented")) {
					int headersize = 2;
					int segsize = 1022;
					//A rather dumb hack. If the request contains just one byte, take that as sequence number
					//Else, expect string in form of S<sequenceNumber>
					int sequence = 0;
					if(exchange.getRequestPayload().length == 1) {
						sequence  = exchange.getRequestPayload()[0];
					}else if(exchange.getRequestPayload().length > 1) {
						sequence = Integer.parseInt(exchange.getRequestText().replace("S", ""));
					}else if(exchange.getQueryParameter("segment") != null) {
						sequence = Integer.parseInt(exchange.getQueryParameter("segment"));
					}
				
					/*try{
						sequence = Integer.parseInt(request);
					}catch(NumberFormatException ex) {}*/
					int maxSeq = (int) Math.ceil((float)data.length / (float)segsize);
					int start = sequence*segsize;
					int end = Math.min(data.length - 1 , start + segsize);
					int len = headersize + end - start;
					int dlen = end - start;
					byte[] out = new byte[len];
					out[0] = (byte)sequence;
					out[1] = (byte)maxSeq;
					for(int i=0; i<dlen; i++) {
						out[i+2] = data[start+i];
					}
					
					exchange.respond(ResponseCode.CONTENT, out);
					return;
				}else {
					exchange.respond(content);
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				exchange.respond(ResponseCode.INTERNAL_SERVER_ERROR);
			}			
		}
		
		@Override
		public void handlePUT(CoapExchange exchange) {
			this.changed();
			handleOrgSpecResource(exchange);
		}		
		
		@Override
		public void handleGET(CoapExchange exchange) {
			if((this.getName().equals("org-spec") || this.getName().equals("org-spec-segmented"))) {
				handleOrgSpecResource(exchange);
			}
			
			if(this.getName().equals("org-entities")) {
				exchange.respond(ResponseCode.CONTENT, "org-entities");
				return;
			}
			
			if(!(this.getName().equals("org-spec") || this.getName().equals("org-spec-segmented"))) {
				exchange.respond(ResponseCode.METHOD_NOT_ALLOWED);
				return;
			}
			// respond to the request
			try {
				String content = Files.readString(Path.of(Launcher.fileName));
				System.out.printf("ORG-SPEC: Responding with whole content of %d\n", content.length());
				
				exchange.respond(content);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				exchange.respond(ResponseCode.INTERNAL_SERVER_ERROR);
			}
			
		}	
	}
	
	static class OEResource extends CoapResource {
		private GroupInstance groupInstance;
		private SchemeInstance schemeInstance;
		private Role role;
		private RolePlayer rolePlayer;
		private OEAgent agent;
		private GoalInstance goalInstance;
		private GoalCommitment goalCommitment;
		private String resource = null;
		private int currentReward = 5;
		
		private static class GoalCommitment{
			public GoalInstance goal;
			public OEAgent agent;
			public GoalCommitment(GoalInstance goal, OEAgent agent) {
				this.goal = goal;
				this.agent = agent;
			}
		}
		
		public OEResource(GoalCommitment goalCommitment) {	
			super(goalCommitment.goal.getId() + "_" + goalCommitment.agent.getId());
			this.goalCommitment = goalCommitment;
			goalCommitment.goal.committed(goalCommitment.agent);
		}
		
		public OEResource(GroupInstance gi) {
			super(gi.getId());
			this.groupInstance = gi;
			for(Role role : gi.getGrSpec().getRoles()) {
				add(new OEResource(gi, role));
			}
		}
		
		public OEResource(SchemeInstance si) {
			super(si.getId());
			this.schemeInstance = si;
		}
		
		public OEResource(OEAgent agent) {
			super(agent.getId());
			this.agent = agent;
		}
		
		public OEResource(GoalInstance goalInstance) {
			super(goalInstance.getId() + "_" + goalInstance.getSpec().getId());
			this.goalInstance = goalInstance;
		}
		
		public OEResource(String id) {
			super(id);
		}
		
		public OEResource(GroupInstance groupInstance, Role role) {
			super(groupInstance.getId() + "_" + role.getId());
			setObservable(true);
			//resource = String.format("reward:%s;id:%s", 5 - this.getChildren().size(), role.getId());
			this.groupInstance = groupInstance;
			this.role = role;
		}
		
		public OEResource(RolePlayer rolePlayer, int reward) {
			super("rp" + "_" + rolePlayer.getPlayer().getId() + "_" + rolePlayer.getGroup().getId() + "_" + rolePlayer.getRole().getId());
			setObservable(true);
			this.rolePlayer = rolePlayer;
			currentReward = reward;
			
			for(Permission p : rolePlayer.getPermissions()) {
				try {
					MissionPlayer mp = rolePlayer.getPlayer().commitToMission(p.getMission().getId(), p.getScheme());

				} catch (MoiseConsistencyException | MoiseCardinalityException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			
			for(GoalInstance gi : rolePlayer.getPlayer().getPossibleGoals()) {
				gi.uncommitted(rolePlayer.getPlayer());
				oegoals.add(new OEResource(gi));

			}
		}
		
		@Override
		public void handlePOST(CoapExchange exchange) {
			String data = exchange.getRequestText();
			if(groupInstance != null && role != null) {
				
				//AgentResource ag = new AgentResource(exchange.getRequestText());
				try {
					OEAgent agent = orgEntity.getAgent(data);
					if(agent == null) {
						agent = orgEntity.addAgent(data);
						agents.add(new OEResource(agent));
					}
					System.out.printf("Agent %s wants to adopt role %s in %s\n", agent.getId(), role.getId(), groupInstance.getId());
					RolePlayer rp = agent.adoptRole(role.getId(), groupInstance);
					currentReward = 5 - this.getChildren().size();
					this.add(new OEResource(rp, currentReward));
					exchange.respond(ResponseCode.CREATED);
					System.out.println("Added agent to group");
					return;
					
				} catch (MoiseException e) {
					//e.printStackTrace();
					System.out.println(e.getMessage());
					exchange.respond(ResponseCode.BAD_REQUEST, e.getMessage());
				}
				
			}
			else if(goalInstance != null) {
				OEAgent agent = orgEntity.getAgent(data);
				//goalInstance.committed(agent);
				add(new OEResource(new GoalCommitment(goalInstance, agent)));
				exchange.respond(ResponseCode.CREATED);
				return;
			}
			
		}		
		
		
		
		@Override
		public void handleGET(CoapExchange exchange) {
			if(rolePlayer != null) {
				boolean wf = rolePlayer.getGroup().isWellFormed();
				Object rs = rolePlayer.getGroup().getRespSchemes();
				Object scms = rolePlayer;
				Object mis = rolePlayer.getPlayer().getMissions();
				Object gls = rolePlayer.getPlayer().getPossibleGoals();
				Object obg = rolePlayer.getObligations();
				Object per = rolePlayer.getPermissions();
				rolePlayer.getPlayer().getLeafestPossibleGoal();
				String response = "";
				for(GoalInstance gi : rolePlayer.getPlayer().getPossibleGoals()) {
					response += gi.getId() + " ";
				}
				//response  = gson.toJson(rolePlayer.getPlayer().getPossibleGoals());
				if(resource != null) {
					response = String.format("%s;rid:%s;aid:%s", resource, rolePlayer.getRole().getId(), rolePlayer.getPlayer().getId());
				}else {
					response = String.format("reward:%s;rid:%s;aid:%s", currentReward, rolePlayer.getRole().getId(), rolePlayer.getPlayer().getId());
				}
				System.out.printf("Roleplayer %s requested status of %s\n", rolePlayer.getPlayer().getId(), rolePlayer.getRole().getId());
				exchange.respond(ResponseCode.CONTENT, response);
				return;
			}
			else if(goalInstance != null) {
				String response = "";
				response  = gson.toJson(goalInstance.getArgumentValues());
				ArrayList<String> commitedAgents = new ArrayList<String>();
				for(OEAgent ag : goalInstance.getCommittedAgents()) {
					commitedAgents.add(ag.getId());
				}
				response += gson.toJson(commitedAgents);
				response += gson.toJson(goalInstance.getState());
				exchange.respond(ResponseCode.CONTENT, response);
				return;
			}
			else if(groupInstance != null && role != null) {
				if(resource != null) {
					String response = String.format("%s;rid:%s", resource, role.getId());
					exchange.respond(response);
					return;
				}
				Object roleprop = role.getProperties();
				//reward:5;id:role_comfort_sensor
				String response = String.format("reward:%s;rid:%s", 5 - this.getChildren().size(), role.getId());
				exchange.respond(ResponseCode.CONTENT, response);
				System.out.printf("Agent wants to know status of %s : %s\n", groupInstance.getId(), role.getId());
				return;
			}
			exchange.respond(ResponseCode.CONTENT,this.getName());
		}
		
		@Override
		public void handlePUT(CoapExchange exchange) {
			resource = exchange.getRequestText();
			if(resource.length() == 0) {
				resource = null;
			}else {
				currentReward = Integer.parseInt(resource.split(":")[1]);
			}
			// respond to the request
			exchange.respond(ResponseCode.CHANGED);
			changed();
			oegroups.updateResource(this.getPath());
			oegroups.changed();
		}
		
		@Override
		public void handleDELETE(CoapExchange exchange) {
			if(rolePlayer != null) {
				try {
					OEAgent ag = rolePlayer.getPlayer();
					
					for(MissionPlayer mp : ag.getMissions()) {
						try {
							ag.abortMission(mp.getMission().getId(), mp.getScheme() );
						} catch (MoiseException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					System.out.printf("Agent %s wants to exit role %s\n", agent.getId(), role.getId());
					currentReward = 5;
					rolePlayer.getPlayer().removeRole(rolePlayer.getRole().getId(), rolePlayer.getGroup());
					exchange.respond(ResponseCode.DELETED);
				} catch (MoiseConsistencyException e) {
					// TODO Auto-generated catch block
					System.out.println(e.getMessage());
					//e.printStackTrace();
					exchange.respond(ResponseCode.INTERNAL_SERVER_ERROR);
				}
			}
			else if(goalCommitment != null) {
				OEAgent agent = goalCommitment.agent;
				goalCommitment.goal.uncommitted(agent);
				exchange.respond(ResponseCode.DELETED);
			}

			this.delete();
		}
	}
	
	static class OSResource extends CoapResource {
		private MoiseElement moiseElement;
		
		public OSResource(String id, MoiseElement osElement) {
			super(id);
			this.moiseElement =  osElement;
		}
		
		public OSResource(Role role) {
			this("os_"  + role.getId(), role);
			moiseElement = role;
		}
		
		public OSResource(Scheme scheme) {
			this("os_"  + scheme.getId(), scheme);
			moiseElement = scheme;
		}
		
		public OSResource(Group group) {
			this("os_"  + group.getId(), group);
	  		moiseElement = group;
			//for(Group g: group.getAllSubGroupsTree()) {
			//	this.add(new OSResource(g));
			//}
			for(Role r: group.getRoles()) {
				this.add(new OSResource("os_" + group.getId() + "_" + r.getId(), r));
			}
		}
		


		@Override
		public void handleGET(CoapExchange exchange) {

			// respond to the request
			String response = "";
			for(String key : moiseElement.getProperties().keySet()) {
				response += key + ":" + moiseElement.getProperty(key) + " ";
			}
			exchange.respond(response);
		}		

	}

	static class PubSubResource extends CoapResource {

		private volatile String resource = "test measurement";

		public PubSubResource(String name) {

			// set resource identifier
			super(name);
			setObservable(true);
			// set display name
			getAttributes().setTitle("pub-sub Resource");
		}
		
		public PubSubResource(String name, String resource) {
			this(name);
			this.resource = resource;
		}
		
		public void updateResource(String resource) {
			System.out.println("Received PUT request for measurements");
			this.resource = resource;
			changed();
		}

		@Override
		public void handleGET(CoapExchange exchange) {

			// respond to the request
			System.out.println("Received GET request for measurements");
			exchange.respond(resource);
		}

		@Override
		public void handlePUT(CoapExchange exchange) {

			resource = exchange.getRequestText();
			System.out.println("Received update:" + resource);
			// respond to the request
			exchange.respond(ResponseCode.CHANGED);
			changed();
		}
		
		@Override
		public void handlePOST(CoapExchange exchange) {
			resource = exchange.getRequestText();
			// respond to the request
			exchange.respond(ResponseCode.CHANGED);
			changed();
		}
	}
}
