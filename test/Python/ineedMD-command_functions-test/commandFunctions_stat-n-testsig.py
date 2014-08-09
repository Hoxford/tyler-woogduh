import serial
import time
import sys
sys.path.append("~/Dropbox/ineedMD Test/Simulators/Python Simulator/SerialCommunication")
import commands

#ser = serial.Serial(port ='/dev/tty.usbserial', baudrate =115200)
ser = serial.Serial(port = 6, baudrate =115200)
print "opened port and running main()"


ser.timeout = 1

#1 STATUS REQUEST
def get_status():
    
    ser.write('\x9C\x01\x05\x11\xC9')
    #print "Get Status Command - 9C 01 05 11 C9"
    print "requesting status..."

def get_testSignal():
    ser.write('\x9C\x01\x06\x18\x01\C9')
    print "requesting test signal"
    

#helper function to reveal important bit to analyze in the individual functions
def andHex(operatingModeByte, val2):
    andedValHex = hex(int(operatingModeByte, base=16) & val2)
    andedValBin = bin(int(andedValHex, 16)) [2:]

    desiredSize = (len(operatingModeByte)-2) *4

    global andedValBin0
    andedValBin0 = andedValBin.zfill(desiredSize)
    
    return andedValBin0

####functions to help identify diff status 
def power():
    
    imptBit = andHex(operatingModeByte,0x60) [1:3]
    
    if imptBit == '00':
        power_state = "hibernate"
        print "power state = " + power_state
    elif imptBit == '01':
        power_state = "sleep"
        print "power state = " + power_state
    elif imptBit == '10':
        power_state = 'active'
        print "power state = " + power_state
    else:
        power_state = "high power"
        print "power state = " + power_state

def capture():

    imptBit = andHex(operatingModeByte, 0x10) [3:4]

    if imptBit == '0':
        capture_state = "False"
        print "Capturing EKG data to internal flash = " + capture_state
    else:
        capture_state = "True"
        print "Capturing EKG data to internal flash = " + capture_state

def stream():

    imptBit =  andHex(operatingModeByte, 0x08) [4:5]
    
  
    if imptBit == '0':
        stream_state = "False"
        print "Streaming EKG data through bT = " + stream_state
    else:
        stream_state = "True"
        print "Streaming EKG data through bT = " + stream_state

def error():

    imptBit = andHex(operatingModeByte, 0x04) [5:6]

    if imptBit == "0":
        error_state = "False"
        print "No error detected"
    else:
        error_state = "True" 
        print "Error detected"

def usb():
    
    imptBit = andHex(operatingModeByte, 0x02) [6:7]
    if imptBit == "0":
        USB_state = "False"
        print "USB connected = " + USB_state
    else:
        USB_state = "True"
        print "USB connected = " + USB_state

#### combines all helper functions into one parser

def parseOpModeByte(operatingModeByte):
    global power_state
    global capture_state
    global stream_state
    global error_state
    global USB_state

    power()
    capture()
    stream()
    error()
    usb()

### helper functions for alarm byte parser
def pacerSusp():
    imptBit = andHex(alarmByte, 0x80) [0:1]
    if imptBit == "0":
        pacerSusp_state = "False"
        print "Pacer suspected = " + pacerSusp_state
    else:
        pacerSusp_state = "True"
        print "Pacer suspected = " + pacerSusp_state

def RAlead():
    imptBit = andHex(alarmByte, 0x40) [1:2]
    if imptBit == "0":
        RAlead_state = "ON"
        print "RA Lead " + RAlead_state
    if imptBit == "1":
        RAlead_state = "OFF!"
        print "RA Lead " + RAlead_state

def LAlead():
    imptBit = andHex(alarmByte, 0x20) [2:3]
    if imptBit == "0":
        LAlead_state = "ON"
        print "LA Lead " + LAlead_state
    if imptBit == "1":
        LAlead_state = "OFF!"
        print "LA Lead " + LAlead_state

def RLlead():
    imptBit = andHex(alarmByte, 0x10) [3:4]
    if imptBit == "0":
        RLlead_state = "ON"
        print "RL Lead " + RLlead_state
    if imptBit == "1":
        RLlead_state = "OFF!"
        print "RL Lead " + RLlead_state

def LLlead():
    imptBit = andHex(alarmByte, 0x08) [4:5]
    if imptBit == "0":
        LLlead_state = "ON"
        print "LL Lead " + LLlead_state
    if imptBit == "1":
        LLlead_state = "OFF!"
        print "LL Lead " + LLlead_state

def V1lead():
    imptBit = andHex(alarmByte, 0x04) [5:6]
    if imptBit == "0":
        V1lead_state = "ON"
        print "V1 Lead " + V1lead_state
    if imptBit == "1":
        V1lead_state = "OFF!"
        print "V1 Lead " + V1lead_state

def V2lead():
    imptBit = andHex(alarmByte, 0x02) [6:7]
    if imptBit == "0":
        V2lead_state = "ON"
        print "V2 Lead " + V2lead_state
    if imptBit == "1":
        V2lead_state = "OFF!"
        print "V2 Lead " + V2lead_state

def V3lead():
    imptBit = andHex(alarmByte, 0x01) [7:8]
    if imptBit == "0":
        V3lead_state = "ON"
        print "V3 Lead " + V3lead_state
    if imptBit == "1":
        V3lead_state = "OFF!"
        print "V3 Lead " + V3lead_state
        
def V4lead():
    imptBit = andHex(alarmByte1, 0x80) [0:1]
    if imptBit == "0":
        V4lead_state = "ON"
        print "V4 Lead " + V4lead_state
    if imptBit == "1":
        V3lead_state = "OFF!"
        print "V4 Lead " + V4lead_state

def V5lead():
    imptBit = andHex(alarmByte1, 0x40) [1:2]
    if imptBit == "0":
        V5lead_state = "ON"
        print "V5 Lead " + V5lead_state
    if imptBit == "1":
        V5lead_state = "OFF!"
        print "V5 Lead " + V5lead_state

def V6lead():
    imptBit = andHex(alarmByte1, 0x20) [2:3]
    if imptBit == "0":
        V6lead_state = "ON"
        print "V6 Lead " + V6lead_state
    if imptBit == "1":
        V6lead_state = "OFF!"
        print "V6 Lead " + V6lead_state



### combines al helper functions into one alarm block parser
def parseAlarmByte(alarmByte):
    global pacerSusp_state
    global RA_state
    global LA_state
    global RL_state
    global LL_state
    global V1_state
    global V2_state
    global V3_state

    pacerSusp()
    RAlead()
    LAlead()
    RLlead()
    LLlead()
    V1lead()
    V2lead()
    V3lead()
    V4lead()
    V5lead()
    V6lead()


def parse_status():
 #   time.sleep(3)
    if ser.inWaiting() != 0:
        global response
        response = ser.readline()
        print "RESPONSE RECEIVED: " + response
        if response[:2] == '9C':            
            if response[2:4] == ' 2':
                
                if response[5:7] == '11':
                    
                    global operatingModeByte
                    operatingModeByte = '0x' + response [13:15]
                    print ""
                    print "THE CURRENT OPERATING STATUS OF THE DONGLE IS AS FOLLOWS: "
                    parseOpModeByte(operatingModeByte)
                    print "END OF OPERATING STATUS DISPLAY"
                    print ""
                    global alarmByte
                    alarmByte = '0x' + response [32:34]

                    global alarmByte1
                    alarmByte1 = '0x' + response [35:37]
                    
                    print "ALARM STATUS IS AS FOLLOWS: "
                    parseAlarmByte(alarmByte)
                    print "END OF ALARM STATUS DISPLAY"
                    print ""
    

# MAIN 
def main():
    global timeSet
    timeSet = 'False'   

    ser.flushInput()
    entry = raw_input('Enter any key... ')
    #ser.readline()
    i = 0
    
    while i < 1:
        
                
            get_status() #0x11
            time.sleep(1)
            parse_status()
            

            get_testSignal()
            time.sleep(1)
            parse_status()
    

            i += 1

  
    #ser.flushInput()
    exit = raw_input('Enter any key to exit... ')
    #

    ser.close()
    
if  __name__ =='__main__':
    main()



    
