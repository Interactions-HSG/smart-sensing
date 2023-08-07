package controller_hardware;

import cartago.Artifact;
import cartago.OPERATION;
import jade.util.Logger;

public class ControlProgram extends Artifact {
    int inputUpdates = 0;
    long start = 0;

    @OPERATION
    void activate(){
        start = System.currentTimeMillis();
        inputUpdates = 0;
    }

    @OPERATION
    void deactivate(){
        this.log(String.format("Got %d updates in %d seconds", inputUpdates, (System.currentTimeMillis() - start)/1000));
        Logger.println("hello log");
    }
    @OPERATION
    void processInput(){
        inputUpdates++;
    }
}
