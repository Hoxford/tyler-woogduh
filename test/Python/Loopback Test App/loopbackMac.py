import serial
import time


ser = serial.Serial(port ='/dev/tty.usbserial', baudrate =115200)
print "opened port"
##
ser.timeout = 1
##
loop = 100
receivecount = 0
waitcount = 0

##
ser.flushInput()

while ser.inWaiting() == 0:
    
    waitcount = 1
    #print waitcount
    if waitcount > 1000:
        "made it here"
        ser.close()
        
        
incoming = ser.readline()
#print "read one line"

while loop >0:
    

##    while ser.inWaiting() == 0:
##        timeoutcount +=1
##        if timeoutcount >100000:
##            print "timeout"
##            ser.close() 68

    if incoming == 'NO CARRIER 0 ERROR 0 \r\n':
        print "read two more"
        incoming2 = ser.readline()
        incoming3 = ser.read(26)
        receivecount += 1
        print incoming3
        print receivecount
        print " "
        ser.write(incoming3)


    elif incoming == 'RING 0 7c:05:07:e9:e9:a6 1 RFCOMM \r\n':
        #print "read again"
        incoming2 = ser.read(26)
        receivecount += 1
        print incoming2
        print receivecount
        print" "
        ser.write(incoming2)

    else:
        print "3"
        print incoming
        receivecount += 1
        print receivecount
        print " "
        
        ser.write(incoming)

    loop -=1
print "out of loop"
ser.close()


