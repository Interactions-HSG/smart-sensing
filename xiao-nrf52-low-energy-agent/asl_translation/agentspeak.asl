!measure_temperature.

// Plan to accomplish the goal when energy is available
+!measure_temperature : energy_buffer_high
<-  read_sensor; 
    transmit_data;
    sleep(measurement_interval).
    
// Plan to handle low energy in buffer
+!measure_temperature: energy_buffer_low
<-  //Ask other agents to achieve the goal
    tell(all, achieve, measure_temperature);
    -!measure_temperature; //Remove the goal
    sleep. //Sleep till next energy update

// React to update about available energy
+energy_buffer_high : measurement_required
<-  tell(all, energy_buffer_high); 
    //Add goal to accomplish
    !!measure_temperature.

// Remove goal and tell other agents about low energy
+energy_buffer_low : measurement_required
<-  -!measure_temperature; //Remove goal
    tell(all, energy_buffer_low);
    sleep.

// React to other agent's low energy by taking over goal
+energy_buffer_low[source=agent]: agent not self
<- !!measure_temperature. //Add goal

// Default plan when buffer is low
+energy_buffer_low <- sleep.