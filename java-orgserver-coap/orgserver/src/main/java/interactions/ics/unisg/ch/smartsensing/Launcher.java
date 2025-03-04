package interactions.ics.unisg.ch.smartsensing;

import java.net.SocketException;

import org.eclipse.californium.core.config.CoapConfig;
import org.eclipse.californium.elements.config.Configuration;
import org.eclipse.californium.elements.config.TcpConfig;
import org.eclipse.californium.elements.config.UdpConfig;

public class Launcher {

	//Only for demo purpose!
	//Load the sample MOISE organization specification
	public static String fileName = "org.xml";

	static {
		CoapConfig.register();
		UdpConfig.register();
		TcpConfig.register();
	}

	public static void main(String[] args) {
		try {
			//Setup the knowledge graph. OSHES is loaded on start.
			RDFTripleStore knowledgeGraph = new RDFTripleStore();
			knowledgeGraph.init();

			//Initialize the CoAP Server
			boolean udp = true;
			boolean tcp = false;
			//System.out.println("Working Directory = " + System.getProperty("user.dir"));
			int port = Configuration.getStandard().get(CoapConfig.COAP_PORT);
			if (0 < args.length) {
				tcp = args[0].equalsIgnoreCase("coap+tcp:");
				if (tcp) {
					System.out.println("Please Note: the TCP support is currently experimental!");
				}
			}
			if(args != null && args.length > 0) {
				fileName = args[0];				
			}
			
			//System.out.printf("Intializing OrgServer from %s\n", fileName);
			//MoiseOrgServer server = new MoiseOrgServer(udp, tcp, port,fileName);

			AGRServer server = new AGRServer(udp, tcp, port);
			server.start();

			//TestClient.createDemoRole();
			//TestClient.testConcurrentGets();

		} catch (SocketException e) {
			System.err.println("Failed to initialize server: " + e.getMessage());
		}
	}
}
