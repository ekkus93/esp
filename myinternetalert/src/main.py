from machine import Pin, Timer, disable_irq, enable_irq
from time import sleep
import micropython
import gc
from connect import doConnect
from mia_network import pingMyInternetAlert, postData
from led import *
from config import *

micropython.alloc_emergency_exception_buf(100)

pingFlag = False
postFlag = False
pingTimer = None
postTimer = None
    
def doPing():
    global pingFlag
    
    if pingFlag:
        print('start doPing')
        redLedOn()
        pingMyInternetAlert(quiet=True)

        gc.mem_free()
        redLedOff()
        pingFlag = False
        print('end doPing')
    
def doPost():
    global postFlag
    
    if postFlag:
        print('start doPost')
        whiteLedOn()
        postData()
        
        gc.mem_free()
        whiteLedOff()
        postFlag = False
        print('end doPost')
    
def handlePingTimer(timer):
    global pingFlag
    pingFlag = True
    
def handlePostTimer(timer):
    global postFlag
    postFlag = True
    
def initTimers():
    global pingFlag, postFlag, pingTimer, postTimer
    print("start initTimers")
    state = disable_irq()
    pingFlag = True
    postFlag = True
    
    pingTimer = Timer(INTERRUPT["ping"]["timer"])
    postTimer = Timer(INTERRUPT["post"]["timer"])
    
    pingPeriod = INTERRUPT["ping"]["interval"]
    pingTimer.init(period=pingPeriod, mode=Timer.PERIODIC, callback=handlePingTimer)
    
    postPeriod = INTERRUPT["post"]["interval"]
    postTimer.init(period=postPeriod, mode=Timer.PERIODIC, callback=handlePostTimer)
    enable_irq(state)
    print("end initTimers")
    
def init():
    redLedOff()
    whiteLedOff()
    initTimers()

def main():
    init()

    while True:
        print(pingFlag, postFlag)
        doConnect(doPost)
        if pingFlag:
            doPing()
        elif postFlag:
            doPost()
        else:
            print(gc.mem_free())
            gc.mem_free()
            sleep(MAIN["loop"]["interval"])       
    
print("starting")
main()



