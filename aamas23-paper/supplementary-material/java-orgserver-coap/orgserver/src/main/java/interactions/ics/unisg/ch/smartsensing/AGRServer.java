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
import interactions.ics.unisg.ch.smartsensing.entities.*;

import com.google.gson.Gson;
public class AGRServer extends CoapServer{

    public AGRServer(boolean udp, boolean tcp, int port) throws SocketException {
        addEndpoints(udp, tcp, port);
        Group group = new Group("room1");
        add(group);
        System.out.printf("Server is listening on port %d\n", port);
    }

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






}
