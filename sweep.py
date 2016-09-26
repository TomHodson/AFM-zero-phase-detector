import numpy as np
from time import time, sleep

import serial
port = serial.Serial('/dev/cu.usbserial', 9600)

from math import sin, pi
def sweep(low, high, f = 0.1, sweep_time = 10, delay = 0.1):
    t0 = time()
    while (time() - t0) < sweep_time:
        t = time()
        val = int(low + (high - low) * 0.5*(sin(2*pi*f*t) + 1))
        #print(val)
        msg = str(val) + "\n"
        port.write(msg.encode())
        sleep(delay)

def set(f):
	msg = str(f) + "\n"
	port.write(msg.encode())


print("example: sweep(low = 31000, high = 35000, f = 0.1, sweep_time = 5, delay = 0.1)")