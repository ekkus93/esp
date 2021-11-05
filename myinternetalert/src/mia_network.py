import ping
import urequests
import network
import time
import ubinascii
import gc

def getMAC():
    return ubinascii.hexlify(network.WLAN().config('mac')).decode()

def getCurrTicks():
    return int(time.ticks_ms()/1000)

def getGateway():
    sta_if = network.WLAN(network.STA_IF)
    result = sta_if.ifconfig()
    
    if len(result) >= 3:
        return result[2]
    
    return None

def getGoogle():
    r = urequests.get('https://www.google.com')
    r.close()
    return r

def pingMyInternetAlert(quiet=False):
    ping.ping("myinternetalert.com", 1, 1000, quiet=quiet, size=8)
    
def postData():
    gateway = getGateway()
    if gateway is None:
        raise Exception("no gateway")
    
    uptime = getCurrTicks()
    mac = getMAC()
    
    data = {
        "pinger":"01",
        "register":"01",
        "gateway": gateway,
        "wificheck":"01",
        "rel":"5.4.83+",
        "sigled":"01",
        "disk":"19",
        "uptime": uptime,
        "load":"35",
        "cpu":"esp32"
        }
    
    headers = {
        "x-mina-mac": mac,
        'Content-Type': 'application/json'
        }
    
    try:
        r = urequests.request("POST", "https://myinternetalert.com/api/register", json = data, headers = headers)
        print(r.content)
        r.close()
        return r
    except Exception as e:
        raise e
    finally:
        gc.collect()
        



