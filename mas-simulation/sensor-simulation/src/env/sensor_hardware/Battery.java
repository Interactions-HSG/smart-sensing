
package sensor_hardware;

import cartago.*;
import java.util.*;

public class Battery extends Artifact {

	public class LevelUpdater extends TimerTask{

		@Override
		public void run() {
			ObsProperty prop = getObsProperty("bat_level");
			if(prop.intValue() > 0){
				prop.updateValue(prop.intValue()-1);
			}
		}
		
	}

	TimerTask timerTask;
	Timer timer;

	int inputPower = 0;
	int outputPower = 0;

	int currentLevel = 100;

	void init(int initialValue) {
		defineObsProperty("bat_level", initialValue);
        defineObsProperty("input_power", 0);
        defineObsProperty("output_power", 0);
		currentLevel = initialValue;
		//timerTask = new LevelUpdater();
		//timer = new Timer();
		//timer.scheduleAtFixedRate(timerTask, 0, 1000);
	}


	@OPERATION
	void charge(int value) {
		inputPower = value;
	}

	@OPERATION
	void discharge(int value) {
		outputPower = value;
	}

	@OPERATION
	void update_battery_state() {
        ObsProperty propL = getObsProperty("bat_level");
		ObsProperty propI = getObsProperty("input_power");
        while(true){
			inputPower = (int) (10 * Math.random());
			propI.updateValue(inputPower);
			propI.commitChanges();
            if(outputPower > inputPower && currentLevel > 0){
				currentLevel--;
                propL.updateValue(currentLevel);
                propL.commitChanges();
            } 
            if(outputPower < inputPower && currentLevel < 100){
				currentLevel++;
                propL.updateValue(currentLevel);
                propL.commitChanges();
            }
			signal("tick");
            await_time(1000);
        }		
	}  
    


	@OPERATION
	void getCharge(int inc, OpFeedbackParam<Integer> newValueArg) {
		ObsProperty prop = getObsProperty("bat_level");
		int newValue = prop.intValue()+inc;
		prop.updateValue(newValue);
		newValueArg.set(newValue);
	}

}
