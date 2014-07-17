import time
import serial
import random
import datetime

#configure serial port and open it
#defaults to port COM9
ser = serial.Serial(port = 7, baudrate = 115200)
ser.timeout = 5
ser.flushInput()

rxCount = 0
txCount = 0
print "open" 
time.sleep(1)

#send 100 char
sum = 0
while txCount < 100:
    #send random char
    rand = chr(random.randint(33,126))
    #rand = "abcdefghij"
    #print "Sent " + repr(rand)
    
    txCount += 1
   
    tstart = datetime.datetime.now()
    #ser.write(rand)
    #ser.write('\x9C\x01\x11\x01\x64\x50\x0B\xB8\xD7\x68\x56\x00\xC0\x00\x00\x00\xC9')
    ser.write('abcdefghijklmnopqrstuvwxyz')
    #wait for dongle response
    timeoutCount = 0;
    
    while (ser.inWaiting() < 26):
        timeoutCount +=1
        #if response takes too long
        if timeoutCount > 100000:
            print 'timeout'
            break
    tend = datetime.datetime.now()   #record end time
    tdelta = tend - tstart
    print "time for one loop:" + repr(tdelta.microseconds) + " microseconds"
    sum += tdelta.microseconds
    #read response and display
    if ser.inWaiting() > 0:
        rxString = ser.read(26)
        print "Rec " + repr(rxString)
        rxCount += 1
        
    #print RX/TX counts
    print "TX " + repr(txCount)
    print "RX " + repr(rxCount)
    print " "

#close serial port
print 'done'
print sum/100
ser.close()




    
