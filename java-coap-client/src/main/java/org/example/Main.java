package org.example;

import org.eclipse.californium.core.CoapClient;

public class Main {
    public static void main(String[] args) {
        System.out.println("Hello world!");
        try {
            CoapClient client = new CoapClient("coap://knx-00fd10f1c8e9.local/p/raqs/aqroom");
            System.out.println(client.get().getResponseText());
        }catch(Exception e){
            System.out.println(e);
        }
    }
}