import serial
import time

ser = serial.Serial(port ='/dev/tty.SerialPort', baudrate =115200)
print "opened port and running main()"

ser.timeout = 1

#1 STATUS REQUEST
def get_status():
    
    ser.write('\x9C\x01\x0A\x11\x00\x00\x00\x00\x00\xC9')
   
    print "requesting status..."


#2 STATUS SET
#hibernate, sleep, on, highPower
#need to edit to have capability of setting other statuses besides power state!
def set_pwr(pwr):
    
    if pwr == "hibernate":
        ser.write('\x9C\x01\x12\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
        print "requesting to enter hibernate mode"
    elif pwr == "sleep":
        ser.write('\x9C\x01\x12\x12\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
        print "requesting to enter sleep mode"
    elif pwr == "on":
        ser.write('\x9C\x01\x12\x12\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
        print "requesting to enter active/on mode"
    elif pwr == "highPower":
        ser.write('\x9C\x01\x12\x12\x00\x00\x60\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
        print "requesting to enter highPower mode"
    elif pwr == "test wrong status request":
        ser.write('\x9C\x01\x12\x12\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
        print "not a valid status request"


def set_time(timeSec):
    timeSec2 = str(timeSec)
    if len(timeSec2) != 10:
        print "please recheck your time. should be represented in seconds since 1900"
    else:
        hexTime = hex(timeSec)
        ser.write(r'x9C\x01\x12\x12\x00\x00\x40\x00\x00\x'
                  + hexTime [2:4] + r'\x'+ hexTime [4:6] + r'\x'+ hexTime [6:8] + r'\x'+ hexTime [8:10] + r'\x'
                  + r'\x00\x00\x00\x00\xC9')
        print "changing time " + hexTime

def off_alarm():
    ser.write('\x9C\x01\x12\x12\x00\x00\x60\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xC9')
    print"turning off all alarms"
    


#3 STORED DATA SETS INFORMATION
def get_dataset_info():
    
    ser.write('\x9C\x01\x0A\x13\x00\x00\x00\x00\x00\xC9')
    print "requesting information of data set"
    

#4 DATA SET TRANSFER
def transfer_dataset(datasetID):

    if len(datasetID) != 4:
        print "not a valid data set ID"
    else:
        byte1 = datasetID [:2]
        byte2 = datasetID [2:]
        strToSend = r'x9C\x01\x0A\x14\x00\x00\x00\x' + byte1 + r'\x' + byte2 + r'\xC9'
        print "retrieving data set " + datasetID 
        
#5 ERASE DATA SET
def erase_dataset(datasetID):
    
    if len(datasetID) != 4:
        print "not a valid data set ID"
    else:
        byte1 = datasetID [:2]
        byte2 = datasetID [2:]
        strToSend = r'x9C\x01\x0A\x15\x00\x00\x00\x' + byte1 + r'\x' + byte2 + r'\xC9'
        print "erasing data set " + datasetID 


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
        strToSend = r'x9C\x01\x0A\x16\x00\x00\x'+ byte1 + r'\x' + byte2 + r'\x'+ byte3 + r'\xC9'
        print "6"
    elif len(hexVal) == 5:
        byte1 = '0' + hexVal[:1]
        byte2 = hexVal [1:3]
        byte3 = hexVal [3:5]
        strToSend = r'x9C\x01\x0A\x16\x00\x00\x'+ byte1 + r'\x' + byte2 + r'\x'+ byte3 + r'\xC9'
        print "5"
    elif len(hexVal) == 4:
        byte1 = hexVal [:2]
        byte2 = hexVal [2:4]
        strToSend = r'x9C\x01\x0A\x16\x00\x00\x00\x' + byte1 + r'\x'+ byte2 + r'\xC9'
        print "4"
    elif len(hexVal) == 3:
        byte1= '0' + hexVal[:1]
        byte2 = hexVal[1:3]
        strToSend = r'x9C\x01\x0A\x16\x00\x00\x00\x' + byte1 + r'\x'+ byte2 + r'\xC9'
        print "3"
    elif len(hexVal) == 2:
        strToSend = r'x9C\x01\x0A\x16\x00\x00\x00\x00\x' + hexVal + r'\xC9'
        print "2"
    elif len(hexVal) == 1:
        strToSend = r'x9C\x01\x0A\x16\x00\x00\x00\x00\x0' + hexVal + r'\xC9'
        print "1"



#7 DISPLAY REAL TIME DATA MEASUREMENTS
def get_rtData(TF):
    
    if TF == "T":
        ser.write('\x9C\x01\x0A\x16\x00\x00\x00\x00\xFF\xC9')
        print "begin streaming of real time data"
    elif TF =="F":
        ser.write('\x9C\x01\x0A\x16\x00\x00\x00\x00\x00\xC9')
        print "do not stream real time data"
    else:
        ser.write('\x9C\x01\x0A\x16\x00\x00\x00\x00\x09\xC9')
        print "not a valid input for this function"



# MAIN 
def main():
    
    entry = raw_input('Enter any key... ')
    #get_status()
    
    set_pwr("hibernate")
    set_pwr("sleep")
    set_pwr("on")
    set_pwr("highPower")
    #set_pwr("test wrong status request")

    #set_time(3613939200)

    #off_alarm()

    get_dataset_info()

    #transfer_dataset("A3B0")

    #erase_dataset("11A0")

    get_rtData("T")
    get_rtData("F")
    #get_rtData("A")

    #capture_data(1)
    #capture_data(28)
    #capture_data(2949)
    #capture_data(35785)
    #capture_data(572570)
    #capture_data(9161135)
    

    exit = raw_input('Enter any key to exit... ')

    ser.close()
    
if  __name__ =='__main__':
    main()
