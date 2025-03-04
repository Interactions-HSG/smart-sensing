package org.example;

public class Main {

    public static void main(String[] args) {
        BacNetClient client = new BacNetIpClient("<bind ip>", "<broadcast ip>", "<client device id>");
        client.start();
        Set<Device> devices = client.discoverDevices(5000); // given number is timeout in millis
        for (Device device : devices) {
            System.out.println(device);

            for (Property property : client.getDeviceProperties(device)) {
                System.out.println(property.getName() + " " + client.getPropertyValue(property));
            }
        }

        client.stop();
    }
}
