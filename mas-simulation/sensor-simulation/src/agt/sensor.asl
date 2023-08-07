//bat_level(100).
input_power(0).
output_power(0).

!start.
//!manage_power.


/* Plans */

+!start : true
    <- .print("Starting sensor");
       .date(Y,M,D); .time(H,Min,Sec,MilSec); // get current date & time
       +started(Y,M,D,H,Min,Sec);             // add a new belief
       !!monitor_battery.

+!evaluate:true
    <- .print("Evaluating current state").

+!search_for_roles:true
    <- .print("Looking for roles to play").

+!monitor_battery: true
    <- update_battery_state.

+!manage_power:bat_level(L) & L > 90
    <-  discharge(10);
        .broadcast(tell, sensor_data(10));
        .print("Sense").

+!manage_power: bat_level(L) & L <80
    <- discharge(0);
        .print("Sleep").

+!manage_power: bat_level(L)
    <- .print("idle").

+bat_level[source(A)] <- .print("Battery update from ",A).

+tick: bat_level(L) & input_power(I)
    <-  .print("Battery update ",L, I);
        !manage_power.

{ include("$jacamo/templates/common-cartago.asl") }
{ include("$jacamo/templates/common-moise.asl") }

// uncomment the include below to have an agent compliant with its organisation
//{ include("$moise/asl/org-obedient.asl") }
