import serial
import time


ser = serial.Serial(port ='/dev/tty.usbserial', baudrate =115200)
print "opened port and running main()"

ser.timeout = 1

#1 STATUS REQUEST
def get_status():
    
    ser.write('\x9C\x01\x0A\x11\x00\x00\x00\x00\x00\xC9')
    print "Get Status Command - 9C 01 0A 11 00 00 00 00 00 C9"
    print "requesting status..."


#2 STATUS SET

def set_time(timeSec):
    timeSec2 = str(timeSec)
    if len(timeSec2) != 10:
        print "please recheck your time. should be represented in seconds since 1900"
    else:
        global hexTime
        hexTime = hex(timeSec)[2:10].decode('hex')
        ser.write('\x9C\x01\x12\x12\x00\x00\x40\x00\x00')
        ser.write(hexTime [0:2] + hexTime [2:4] + hexTime [4:6] + hexTime [6:8])
        ser.write('\x00\x00\x00\x00\xC9')
        print "changing time " + hexTime
        print "Setting time now!"
        
        global timeSet
        timeSet = 'True'
        #print timeSet + '1'
        

#hibernate, sleep, on, highPower
def set_pwr(pwr):
    #print timeSet
    
    if timeSet != 'True': #if time has not been set, set time to 0 and set the respective power

        if pwr == "hibernate":
            ser.write('\x9C\x01\x12\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
        
            print "requesting to enter hibernate mode; time 0"
            print "Packet sent for hibernate - 9C 01 12 12 00 00 00 00 00 00 00 00 00 00 00 00 00 C9"
        elif pwr == "sleep":
            ser.write('\x9C\x01\x12\x12\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
            
            print "requesting to enter sleep mode; time 0"
        elif pwr == "on":
            ser.write('\x9C\x01\x12\x12\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
            
            print "requesting to enter active/on mode ; time 0"
        elif pwr == "highPower":
            ser.write('\x9C\x01\x12\x12\x00\x00\x60\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
            
            print "requesting to enter highPower mode; time 0"
        elif pwr == "test wrong status request":
            ser.write('\x9C\x01\x12\x12\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
            
            print "not a valid status request; time 0"

    else: #if time has been set, set respective power with the current time
        if pwr == "hibernate":
            ser.write('\x9C\x01\x12\x12\x00\x00\x00\x00\x00')
            ser.write(hexTime [0:2] + hexTime [2:4] + hexTime [4:6] + hexTime [6:8])
            ser.write('\x00\x00\x00\x00\xC9')
            print "requesting to enter hibernate mode"
        elif pwr == "sleep":
            ser.write('\x9C\x01\x12\x12\x00\x00\x20\x00\x00')
            ser.write(hexTime [0:2] + hexTime [2:4] + hexTime [4:6] + hexTime [6:8])
            ser.write('\x00\x00\x00\x00\xC9')
            print "requesting to enter sleep mode"
        elif pwr == "on":
            ser.write('\x9C\x01\x12\x12\x00\x00\x40\x00\x00')
            ser.write(hexTime [0:2] + hexTime [2:4] + hexTime [4:6] + hexTime [6:8])
            ser.write('\x00\x00\x00\x00\xC9')
            print "requesting to enter active/on mode"
        elif pwr == "highPower":
            ser.write('\x9C\x01\x12\x12\x00\x00\x60\x00\x00')
            ser.write(hexTime [0:2] + hexTime [2:4] + hexTime [4:6] + hexTime [6:8])
            ser.write('\x00\x00\x00\x00\xC9')
            print "requesting to enter highPower mode"
        elif pwr == "test wrong status request":
            ser.write('\x9C\x01\x12\x12\x00\x00\x80\x00\x00')
            ser.write(hexTime [0:2] + hexTime [2:4] + hexTime [4:6] + hexTime [6:8])
            ser.write('\x00\x00\x00\x00\xC9')
            print "not a valid status request"


def off_alarm():
    if timeSet != 'True':
        ser.write('\x9C\x01\x12\x12\x00\x00\x60\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
        print "Packet sent for True Condition - 9C 01 12 12 00 00 60 00 00 00 00 00 00 00 00 00 00 C9"
        print"turning off all alarms; time 0"
    else:
        ser.write('\x9C\x01\x12\x12\x00\x00\x40\x00\x00')
        ser.write(hexTime [0:2] + hexTime [2:4] + hexTime [4:6] + hexTime [6:8])
        ser.write('\x00\x00\x00\x00\xC9')
        print"turning off all alarms"


#3 STORED DATA SETS INFORMATION
def get_dataset_info():
    
    ser.write('\x9C\x01\x0A\x13\x00\x00\x00\x00\x00\xC9')
    print "requesting information of data set"
    print "Packet sent - 9C 01 0A 13 00 00 00 00 00 C9"
    

#4 DATA SET TRANSFER
def transfer_dataset(datasetID):
    datastr = str(datasetID)
    if len(datastr) != 4:
        print "not a valid data set ID"
    else:
        byte1 = datasetID [:2]
        byte2 = datasetID [2:]
        ser.write('\x9C\x01\x0A\x14\x00\x00\x00')
        ser.write(byte1.decode('hex'))
        ser.write(byte2.decode('hex'))
        ser.write('\xC9')
        print "Packet sent - 9C 01 0A 14 00 00 00 "+byte1+" "+byte2+" C9"

#5 ERASE DATA SET
def erase_dataset(datasetID):
    
    if len(datasetID) != 4:
        print "not a valid data set ID"
    else:
        byte1 = datasetID [:2]
        byte2 = datasetID [2:]
        ser.write('\x9C\x01\x0A\x15\x00\x00\x00')
        ser.write(byte1.decode('hex'))
        ser.write(byte2.decode('hex'))
        ser.write('\xC9')
        print "Packet sent - 9C 01 0A 15 00 00 00 "+byte1+" "+byte2+" C9"

#6 CAPTURE DATA SET FOR GIVEN TIME
def capture_data(seconds):
    
    hexVal0 = hex(seconds)
   
    #remove leading 0x
    hexVal = hexVal0[2:]
    
    if len(hexVal) > 6:
        print "desired capture time exceeds max time"
    elif len(hexVal) == 0:
        print "please input a valid time"
    elif len(hexVal) == 6:
        byte1 = hexVal [:2]
        byte2 = hexVal [2:4]
        byte3 = hexVal [4:6]
        ser.write('\x9C\x01\x0A\x16\x00\x00')
        ser.write(byte1.decode('hex'))
        ser.write(byte2.decode('hex'))
        ser.write(byte3.decode('hex'))
        ser.write('\xC9')
        print "command sent to capture data"
    elif len(hexVal) == 5:
        byte1 = '0' + hexVal[:1]
        byte2 = hexVal [1:3]
        byte3 = hexVal [3:5]
        ser.write('\x9C\x01\x0A\x16\x00\x00')
        ser.write(byte1.decode('hex'))
        ser.write(byte2.decode('hex'))
        ser.write(byte3.decode('hex'))
        ser.write('\xC9')
        print "command sent to capture data"
    elif len(hexVal) == 4:
        byte1 = hexVal [:2]
        byte2 = hexVal [2:4]
        ser.write('\x9C\x01\x0A\x16\x00\x00\x00')
        ser.write(byte1.decode('hex'))
        ser.write(byte2.decode('hex'))
        ser.write('\xC9')
        print "command sent to capture data"
    elif len(hexVal) == 3:
        byte1= '0' + hexVal[:1]
        byte2 = hexVal[1:3]
        ser.write('\x9C\x01\x0A\x16\x00\x00\x00')
        ser.write(byte1.decode('hex'))
        ser.write(byte2.decode('hex'))
        ser.write('\xC9')
        print "command sent to capture data"
    elif len(hexVal) == 2:
        ser.write('\x9C\x01\x0A\x16\x00\x00\x00\x00')
        ser.write(hexVal.decode('hex'))
        ser.write('\xC9')
        print "command sent to capture data"
    elif len(hexVal) == 1:
        ser.write('\x9C\x01\x0A\x16\x00\x00\x00\x00')
        ser.write(hexVal)
        ser.write('\xC9')
        print "Packet sent - 9C 01 0A 16 00 00 00 00 31 C9"
        print "1"

#7 DISPLAY REAL TIME DATA MEASUREMENTS
def get_rtData(TF):
    
    if TF == "T":
        ser.write('\x9C\x01\x0A\x17\x00\x00\x00\x00\xFF\xC9')
        print "begin streaming of real time data"
        print "Packet sent - 9C 01 0A 17 00 00 00 00 FF C9"
    elif TF =="F":
        ser.write('\x9C\x01\x0A\x17\x00\x00\x00\x00\x00\xC9')
        print "do not stream real time data"
        print "Packet sent - 9C 01 0A 17 00 00 00 00 00 C9"
    else:
        ser.write('\x9C\x01\x0A\x17\x00\x00\x09\xC9')
        print "not a valid input for this function"



# MAIN 
def main():
    global timeSet
    timeSet = 'False'   

    ser.flushInput()
    
    entry = raw_input('Enter any key... ')
    set_time(3613939200)
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    get_status() #0x11
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    capture_data(1) #0x16
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    off_alarm()#0x12
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    set_pwr("hibernate")
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    set_pwr("sleep")
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    set_pwr("highPower")
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    set_pwr("on")
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter aney key... ')
    get_dataset_info() #0x13
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    transfer_dataset("A3B0")
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()
 
    entry = raw_input('Enter any key... ')
    erase_dataset("11A0")
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    get_rtData("T")
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    get_rtData("F")
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    capture_data(28)
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    capture_data(2949)
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    capture_data(35785)
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    capture_data(572570)
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    capture_data(9161135)
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()

    entry = raw_input('Enter any key... ')
    ser.write('helloworld')
    read = raw_input("Enter any key to read .. ")
    print "DATA READ: " + ser.readline()
 
    
 
 
   
   
 
    
 
  

 
  
    
   
  
  
    #ser.flushInput()
    exit = raw_input('Enter any key to exit... ')
    ser.close()
    
if  __name__ =='__main__':
    main()



    
