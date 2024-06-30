package interactions.ics.unisg.ch.smartsensing;

import java.net.SocketException;

import org.eclipse.californium.core.config.CoapConfig;
import org.eclipse.californium.elements.config.Configuration;
import org.eclipse.californium.elements.config.TcpConfig;
import org.eclipse.californium.elements.config.UdpConfig;

public class Launcher {
	
	public static String fileName = "org.xml";
	
	static {
		CoapConfig.register();
		UdpConfig.register();
		TcpConfig.register();
	}

	public static void main(String[] args) {
		try {
			// create server
			boolean udp = true;
			boolean tcp = false;
			System.out.println("Working Directory = " + System.getProperty("user.dir"));
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
			
			System.out.printf("Intializing OrgServer from %s\n", fileName);
			//MoiseOrgServer server = new MoiseOrgServer(udp, tcp, port);
			AGRServer server = new AGRServer(udp, tcp, port);
			//server.addEndpoints(udp, tcp, port);
			server.start();
			TestClient.createDemoRole();
			//TestClient.testConcurrentGets();

		} catch (SocketException e) {
			System.err.println("Failed to initialize server: " + e.getMessage());
		}
	}
}
