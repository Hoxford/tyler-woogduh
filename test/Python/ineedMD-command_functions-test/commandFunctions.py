import serial
import time

ser = serial.Serial(port ='/dev/tty.usbserial', baudrate =115200)
print "opened port and running main()"

ser.timeout = 1

def get_status():
    ser.write('x9C\x01\x0A\x11\x00\x00\x00\x00\x00\xC9')
   
    print "requesting status..."



#hibernate, sleep, on, highPower
#need to edit to have capability of setting other statuses besides power state!
def set_status(status):
    if status == "hibernate":
        ser.write('x9C\x01\x0A\x12\x00\x00\x00\x00\x00\xC9')
    
        print "requesting to enter hibernate mode"
    elif status == "sleep":
        ser.write('x9C\x01\x0A\x12\x00\x00\x00\x00\x01\xC9')
        print "requesting to enter sleep mode"
    elif status == "on":
        ser.write('x9C\x01\x0A\x12\x00\x00\x00\x00\x10\xC9')
        print "requesting to enter active/on mode"
    elif status == "highPower":
        ser.write('x9C\x01\x0A\x12\x00\x00\x00\x00\x11\xC9')
        print "requesting to enter highPower mode"
    else:
        print "not a valid status request"


def get_dataset_info():
    ser.write('x9C\x01\x0A\x13\x00\x00\x00\x00\x00\xC9')
    print "requesting information of data set"
    

#NOT WORKING.....STRING APPEND ISSUE...
def transfer_dataset(datasetID):
    if len(datasetID) != 4:
        print "not a valid data set ID"
    else:
        byte1 = datasetID [:2]
        byte2 = datasetID [2:]
        strToSend = r'x9C\x01\x0A\x14\x00\x00\x00\x' + byte1 + r'\x' + byte2 + r'\xC9'
        print "retrieving data set " + datasetID 
        

def get_rtData(TF):
    if TF == "T":
        ser.write('x9C\x01\x0A\x16\x00\x00\x00\x00\xFF\xC9')
        print "begin streaming of real time data"
    elif TF =="F":
        ser.write('x9C\x01\x0A\x16\x00\x00\x00\x00\x00\xC9')
        print "do not stream real time data"
    else:
        print "not a valid input for this function"


# MAIN 
def main():
    
    get_status()
    
    set_status("hibernate")
    set_status("sleep")
    set_status("on")
    set_status("highPower")

    get_dataset_info()

    transfer_dataset("A3B0")

    get_rtData("T")
    get_rtData("F")




    ser.close()
    
if  __name__ =='__main__':
    main()
