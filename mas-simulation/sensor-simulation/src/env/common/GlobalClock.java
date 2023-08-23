package common;

public class GlobalClock {
    public static int day;
    public static int hour;
    public static int minute;
    public static int second;
    public static double scaling = 300; // 1 sec = 5 min

    private static Object lock = new Object();
    private static boolean started = false;

    private static boolean stopRequested = false;

    public static void stop(){
        stopRequested = true;
        started = false;
    }
    public static void start(){
        synchronized (lock){
            if(started){
                return;
            }
            Thread thread = new Thread(){
                public void run(){
                    while(!stopRequested){
                        if(second < 59) {
                            second++;
                        }else{
                            second = 0;
                            if(minute < 59){
                                minute++;
                            }else{
                                minute = 0;
                                if(hour < 23){
                                    hour++;
                                }else{
                                    hour = 0;
                                }
                            }
                        }
                        try {
                            Thread.sleep((long) (1000 / scaling));
                        } catch (InterruptedException e) {
                            throw new RuntimeException(e);
                        }
                    }
                }
            };
            thread.start();
            started = true;
            stopRequested = false;
        }
    }
}
