//bat_level(100).
input_power(0).
output_power(0).
offset(10).
state(0).

!start.
//!manage_power.


/* Plans */

+!start : true
    <- .print("Starting sensor");
       .date(Y,M,D); .time(H,Min,Sec,MilSec); // get current date & time
       +started(Y,M,D,H,Min,Sec);             // add a new belief
       !!monitor_battery.

+!search_for_roles:true
    <- .print("Looking for roles to play").

+!monitor_battery: true
    <- update_battery_state.

+!manage_power:bat_level(L) & L > 510
    <- !evaluate.

+!manage_power: bat_level(L) & state(S) & L >=500 & S==1
    <- !sense.

+!manage_power: bat_level(L) & state(S) & S==1 & L < 500
    <- !sleep;
        .print("Sleep").

+!manage_power: bat_level(L) & state(S) & L <=510 & S==0
    <- !sleep.

+!evaluate:true
<- .print("Evaluating");
    updateRoleStatus;
    !decide.

+!decide:current_benefit(CB) & highest_benefit(HB) & CB == 0 & HB == 0
<- !sleep.

+!decide:current_benefit(CB) & highest_benefit(HB) & CB >= HB
<- !sense.

+!decide:current_benefit(CB) & highest_benefit(HB) & CB < HB
<- switchRole;
   !sense.

+!decide:current_benefit(CB) & highest_benefit(HB)
<- .print("Unable to decide with CB=", CB, " and HB=", HB).

+!sense:state(S) & bat_level(L) & temperature(T) & current_role(CR)
    <-
    discharge(0.015); //Each sensing cycle takes 30mJ
    -+state(1);
    .broadcast(tell, sensor_data(T));
    .print("Sense T=", T, " for role ", CR).

+!sleep:state(S)
    <-
    discharge(0);
    -+state(0).

+bat_level[source(A)] <- .print("Battery update from ",A).

+tick: bat_level(L) & input_energy(I) & state(S)
    <-  .print("Battery charge:",L, " Input:", I, " State:", S);
        !manage_power.

{ include("$jacamo/templates/common-cartago.asl") }
{ include("$jacamo/templates/common-moise.asl") }

// uncomment the include below to have an agent compliant with its organisation
//{ include("$moise/asl/org-obedient.asl") }
