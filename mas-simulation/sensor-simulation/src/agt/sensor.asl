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

+!search_for_roles:true
    <- .print("Looking for roles to play").

+!start_simulation: true
    <- simulateSensorState.

+!monitor_organization:true
    <- observeOrganization.

+!manage_power:energy_in_buffer(L) & L > 510 & state(S) & S==0 & engagement(R) & R == 0
    <-
        //.print("Energized! But no job. Buffer=", L);
        !evaluate.

+!manage_power: energy_in_buffer(L) & state(S) & L >=510 & S==0 & current_benefit(CB) & CB > 0
    <-
        //.print("Got energy, have benefit, will do work");
        -+state(1);
        !sense.

+!manage_power: energy_in_buffer(L) & state(S) & L >=500 & S==1 & current_benefit(CB) & CB > 0
    <- //.print("Got energy, have benefit working");
        !sense.

+!manage_power: energy_in_buffer(L) & state(S) & S==1 & L >= 500 & current_benefit(CB) & CB == 0
    <- !sleep.

+!manage_power: energy_in_buffer(L) & state(S) & S==1 & L < 500
    <- !sleep.

+!manage_power: energy_in_buffer(L) & state(S) & L <=510 & S==0
    <- !sleep.

+!evaluate:true
<-  updateCurrentRoleState;
    findAltRole;
    !decide.

+onOrgUpdate:true
    <- //.print("Revaluating");
    !decide.

+!decide:current_benefit(CB) & current_role(CR) & alternative_benefit(AB) & CB == 0 & AB == 0 & engagement(R) & R > 0
<-   //.print("Deciding to exit from ", CR, " since CB=", CB, "  and AB=", AB, " engaged in ", R);
    .print("On the bench");
    nop.

+!decide:current_benefit(CB) & alternative_benefit(HB) & CB >= HB & state(S) & S==0 & engagement(R) & R > 0
<- nop.

+!decide:current_benefit(CB) & alternative_benefit(HB) & CB >= HB & state(S) & S==1 & engagement(R) & R > 0
<- nop.

+!decide:current_role(CR) & alternative_role(AR) & current_benefit(CB) & alternative_benefit(AB) & CB < AB & not CR == AR
<- .print("Deciding to switch from ", CR, "(", CB, ") to ", AR, "(", AB, ")");
    switchRole;
    -+engagement(1).

+!decide:current_benefit(CB) & alternative_benefit(HB) & state(S) & engagement(R)
<- //.print("Unable to decide with CB=", CB, " and HB=", HB, " engagement=", R, " state=", S).
    nop.

+!sense:state(S) & energy_in_buffer(L) & temperature(T) & current_role(CR) & S==1
    <-
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

+tick: energy_in_buffer(L) & energy_input(I) & state(S)
    <-  //.print("Battery charge=",L, " Input=", I, " State=", S);
        !manage_power.

{ include("$jacamo/templates/common-cartago.asl") }
{ include("$jacamo/templates/common-moise.asl") }

// uncomment the include below to have an agent compliant with its organisation
//{ include("$moise/asl/org-obedient.asl") }
