# scripts/monitor_delay.py
import env

# see https://github.com/platformio/platformio-core/issues/3742#issuecomment-1003454439
def wait_for_monitor_port(source, target, env):
    port = env.GetProjectOption("monitor_port")
    if port is None: return

    print(f"Waiting for monitor port {port}...")
    import serial
    while True:
        try:
            serial.Serial(port)
            break
        except:
            pass

env.AddPostAction("upload", wait_for_monitor_port)