package controller_hardware;

import cartago.Artifact;
import cartago.OPERATION;
import cartago.ObsProperty;
import cartago.OpFeedbackParam;
import common.AppLogger;
import jade.util.Logger;

import java.time.LocalDateTime;
import java.util.Date;

public class TaskScheduler extends Artifact {
    int inputPower = 0;
    int outputPower = 0;
    int currentLevel = 100;
    int currentState = 0;

    int sensorDataReceived = 0;

    void init(int initialValue) {
        defineObsProperty("state", initialValue);
        currentState = initialValue;
    }

    @OPERATION
    void update_schedule() {
        ObsProperty propState = getObsProperty("state");
        while(true){
            if((LocalDateTime.now().getMinute() % 2 == 0) && currentState == 0){
                currentState = 1;
                propState.updateValue(1);
            }else if ((LocalDateTime.now().getMinute() % 2 != 0) && currentState == 1){
                currentState = 0;
                propState.updateValue(0);
            }
            propState.commitChanges();
            signal("tick");
            await_time(1000);
        }
    }


}
