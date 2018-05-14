import serial
import time
import sys
from datetime import datetime
ser = serial.Serial('COM6', 9600, timeout=0)
#f = open('file.txt', 'w')
#sys.stdout = f
time.sleep(1)
while 1:
    try:
        print('Hour: '), 
        print(str(datetime.now())),
        print ser.readline()
        time.sleep(60)
    except ser.SerialTimeoutException:
        print('Data could not be read')
  #  time.sleep(10)