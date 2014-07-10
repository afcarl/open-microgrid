#!/usr/bin/python

import sys
import serial
import time

ser = serial.Serial(sys.argv[1], 1200)
ser.setBaudrate(1200)
#time.sleep(0.02)
ser.close()
