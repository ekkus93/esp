import network
from time import sleep
from wifi_settings import WIFI

def tryConnect(sta_if):
    sta_if.connect(WIFI["ssid"], WIFI["password"])
    while not sta_if.isconnected():
        print("trying to connect...")
        sleep(1)
        
    return sta_if

def doConnect(cb=None):
    print('do_connect')
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print('connecting to network...')
        sta_if.active(True)
        connectFlag = False
        while not connectFlag:
            try:
                sta_if = tryConnect(sta_if)
                connectFlag = True
            except Exception as e:
                print(e)
        if cb is not None:
            cb()
                
    print('network config:', sta_if.ifconfig())