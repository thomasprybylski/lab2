import serial
import serial.tools.list_ports
import psutil 
import time
import pandas as pd
import datetime
import struct

# list the ports and choose the best one
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

m5stick = serial.Serial(port.name,115200) # creates and opens the serial port
last_read_time = time.time() # we are keeping track of the last time we read/sent data, so we can do it regularly
event_log = [] # this will hold tuples of data representing time,value events

while m5stick.is_open: # Loop while we have an open serial connection
    try: # we need to handle serial exceptions, e.g., unplugged
        if m5stick.in_waiting > 0: #there is data
            # read a byte at a time
            event_code = m5stick.read(1)

            # If the event code is the same as a newline, it's probably a garbage message
            # The esp sends these at startup, but they all end in \n, so we can detect them
            if event_code[0] == ord('\n'):
                event_log = [] # we drop the log if we get this message, because the esp restarted
                continue
            if event_code[0] == 0xFF:
                break # we are done
            else:
                event_log.append((datetime.datetime.now().timestamp(),event_code[0]))
        
    except Exception as e:
        print(e)
        break

m5stick.close()
event_log = pd.DataFrame(event_log,columns=["timestamp","value"])
event_log.to_csv(f'event_log_{datetime.datetime.now().timestamp()}.csv',index=False)