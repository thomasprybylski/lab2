import serial
import serial.tools.list_ports
import psutil
import time
import pandas as pd
import datetime
import struct
import os

# List available serial ports
ports = serial.tools.list_ports.comports()
port = None
if len(ports) > 1:    
    print("\n".join([f"{x}: {ports[x]}" for x in range(len(ports))]))
    port = ports[int(input("Select a port: "))]
elif len(ports) == 1:
    port = ports[0]
else:
    print("No serial ports found")
    exit()

# Open the selected serial port for communication with M5StickC
m5stick = serial.Serial(port.name, 115200)
last_read_time = time.time()  # Track the last time we sent data
event_log = []  # To store events like button presses/releases

# Loop to send system stats and handle button events
while m5stick.is_open:
    try:
        # Gather system stats (CPU, memory, battery)
        cpu = int(sum(psutil.cpu_percent(interval=0.1, percpu=True)) / psutil.cpu_count())  # Average CPU percent
        mem = int(psutil.virtual_memory().used / psutil.virtual_memory().total * 100)  # Memory usage in percentage
        battery = int(psutil.sensors_battery().percent)  # Battery percentage

        # Pack the stats into a binary format and send them to M5StickC
        m5stick.write(struct.pack("<HBB", cpu, mem, battery))

        # Read events from M5StickC if available
        if m5stick.in_waiting > 0:
            event_code = m5stick.read(1)

            if event_code[0] == ord('\n'):  # Newline character, reset event log
                event_log = []
                continue

            if event_code[0] == 0xFF:  # Quit event
                event_log.append((datetime.datetime.now().timestamp(), "quit"))
                break
            elif event_code[0] == 0x00:  # Button A press event
                event_log.append((datetime.datetime.now().timestamp(), "pressed"))
            elif event_code[0] == 0x01:  # Button A release event
                event_log.append((datetime.datetime.now().timestamp(), "released"))
            else:
                print("Undefined Behavior?")
        
        # Send stats every second
        if time.time() - last_read_time >= 1:
            last_read_time = time.time()

    except Exception as e:
        print(f"Error: {e}")
        break

# Close the serial connection and save the event log to a CSV file
event_log_df = pd.DataFrame(event_log, columns=["timestamp", "value"])
event_log_df.to_csv(os.getcwd() + f'\\event_log_{datetime.datetime.now().timestamp()}.csv', index=False)

# Close the serial connection
m5stick.close()
