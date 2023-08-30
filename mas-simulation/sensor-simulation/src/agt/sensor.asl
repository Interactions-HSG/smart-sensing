state(0).
engagement(0).

!start.
//!manage_power.


/* Plans */

+!start : true
    <- .print("Starting sensor");
       .date(Y,M,D); .time(H,Min,Sec,MilSec); // get current date & time
       +started(Y,M,D,H,Min,Sec);             // add a new belief
       !!monitor_organization;
       !!start_simulation.

+!start_simulation: true
    <- simulateSensorState.

+!monitor_organization:true
    <- observeOrganization.


+!evaluate:true
<-  refreshRoles;
    evaluateCurrentState.

+!manage_power:energy_in_buffer(L) & L > 500 & state(S) & S==0 & current_benefit(CB) & CB <= 0
    <-
        //.print("Energized! But no job. Buffer=", L);
        //!evaluate.
        nop.

+!manage_power: energy_in_buffer(L) & state(S) & L > 500 & S==0 & current_benefit(CB) & CB > 0
    <-
        //.print("Got energy, have benefit, will do work");
        -+state(1);
        !sense.

+!manage_power: energy_in_buffer(L) & state(S) & L >500 & S==1 & current_benefit(CB) & CB > 0
    <- //.print("Got energy, have benefit working");
        !sense.

+!manage_power: energy_in_buffer(L) & state(S) & S==1 & L > 500 & current_benefit(CB) & CB <= 0
    <- !sleep.

+!manage_power: energy_in_buffer(L) & state(S) & S==1 & L < 500
    <- !sleep.

+!manage_power: energy_in_buffer(L) & state(S) & L <=500 & S==0
    <- !sleep.

+!manage_power: current_benefit(CB) & state(S) & CB<=0 & S==0
    <- !sleep.

+onOrgUpdate:true
    <- .print("org state updated").
    //!decide.

+!sense:state(S) & S==1 & temperature(T)
    <-
    .print("Sense T=", T);
    doTask; //Each sensing cycle takes 30mJ
    .broadcast(tell, sensor_data(T)).

+!sleep:state(S) & S==1
    <-
    .print("Will sleep now");
    enterSleepMode;
    -+state(0).

+!sleep:state(S) & S==0
    <- enterSleepMode.

+current_role(CR):true
<- .print("Role changed to ", CR).

//+energy_in_buffer[source(A)] <- .print("Battery update from ",A).

+tick: energy_in_buffer(L) & energy_input(I) & state(S) & current_benefit(CB)
    <-  //.print("Battery charge=",L, " Input=", I, " State=", S);
        !manage_power.

{ include("$jacamo/templates/common-cartago.asl") }
{ include("$jacamo/templates/common-moise.asl") }

// uncomment the include below to have an agent compliant with its organisation
//{ include("$moise/asl/org-obedient.asl") }
