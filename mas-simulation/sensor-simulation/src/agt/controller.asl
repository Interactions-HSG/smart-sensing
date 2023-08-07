!start.

+!start : true
    <- .print("Starting controller");
       .date(Y,M,D); .time(H,Min,Sec,MilSec); // get current date & time
       +started(Y,M,D,H,Min,Sec).

+sensor_data(D)[source(Ag)] :true
    <- .print("Received sensor data ", D, " from ", Ag).

