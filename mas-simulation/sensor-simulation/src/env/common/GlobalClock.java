package common;

public class GlobalClock {
    public static int day;
    public static int hour;
    public static int minute;
    public static int second;

    public static long ticks=0;
    public static double scaling = 60; // 1 sec = 1 min

    private static Object lock = new Object();
    private static boolean started = false;

    private static boolean stopRequested = false;

    public static int simulation_time_step = 1000;

    public static int simulation_window_size = 5*60000; //5 min
    public static int clock_time_step = simulation_time_step / 300; //1 sec = 5 min

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
                                ticks++;
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
                            Thread.sleep(clock_time_step, 333);
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
