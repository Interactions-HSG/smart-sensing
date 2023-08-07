package common;

import cartago.Artifact;
import cartago.OPERATION;

public class AppLogger extends Artifact {

    void init(int initialValue) {

    }
    @OPERATION
    void writeLog(String message) {
        System.out.println(message);
    }
}
