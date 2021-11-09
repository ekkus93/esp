from machine import Pin, Timer, disable_irq, enable_irq
from time import sleep
import micropython
import gc
from connect import doConnect
from mia_network import pingMyInternetAlert, postData
from led import *
from config import *

micropython.alloc_emergency_exception_buf(100)

connectedFlag = False
pingFlag = False
postFlag = False
pingTimer = None
postTimer = None
    
def doPing():
    global pingFlag
    
    ok = True
    if pingFlag:
        print('start doPing')
        try:
            redLedOn()
            pingMyInternetAlert(quiet=True)
            pingFlag = False

            gc.mem_free()
        except Exception as e:
            ok = False
            print("ERROR: ", e)
        finally:
            redLedOff()
            print('end doPing')
        
    return ok
    
def doPost():
    global postFlag
        
    ok = True
    if postFlag:
        print('start doPost')
        try:
            whiteLedOn()
            postData()
            postFlag = False
            
            gc.mem_free()
        except Exception as e:
            ok = False
            print("ERROR: ", e)
        finally:
            whiteLedOff()
            print('end doPost')
            
    return ok
    
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
    global connectedFlag
    
    init()

    while True:
        print(pingFlag, postFlag)
        connectedFlag = doConnect(not connectedFlag, doPost)
        
        if pingFlag:
            connectedFlag = doPing()
        elif postFlag:
            connectedFlag = doPost()
        else:
            print(gc.mem_free())
            gc.mem_free()
            sleep(MAIN["loop"]["interval"])       
    
print("starting")
main()




