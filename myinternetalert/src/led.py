from machine import Pin
from config import LED

redLed = Pin(LED["red"]["pin"], Pin.OUT)
whiteLed = Pin(LED["white"]["pin"], Pin.OUT)

def redLedOn():
    redLed.value(LED["red"]["on"])
    
def redLedOff():
    redLed.value(LED["red"]["off"])
    
def whiteLedOn():
    whiteLed.value(LED["white"]["on"])
    
def whiteLedOff():
    whiteLed.value(LED["white"]["off"])