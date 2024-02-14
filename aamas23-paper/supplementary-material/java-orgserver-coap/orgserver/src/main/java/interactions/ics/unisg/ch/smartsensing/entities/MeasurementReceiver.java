package interactions.ics.unisg.ch.smartsensing.entities;

import com.google.gson.Gson;
import org.eclipse.californium.core.CoapResource;
import org.eclipse.californium.core.coap.CoAP;
import org.eclipse.californium.core.server.resources.CoapExchange;

import java.util.ArrayList;
import java.util.List;

public class MeasurementReceiver extends CoapResource {
    List<Double> measurements = new ArrayList<Double>();
    Gson gson = new Gson();

    public void reset(){
        measurements.clear();
    }

    public int getCount(){
        return measurements.size();
    }

    public MeasurementReceiver(String name) {
        super(name);
        setObservable(true);
    }

    @Override
    public void handleGET(CoapExchange exchange) {
        String data = gson.toJson(measurements);
        exchange.respond(data);
    }

    @Override
    public void handlePOST(CoapExchange exchange) {
        String data = exchange.getRequestText();
        System.out.println("Received measurement:" + data);
        Double meas = gson.fromJson(data, Double.class);
        measurements.add(meas);
        exchange.respond(CoAP.ResponseCode.CREATED);
        changed();
    }

}
