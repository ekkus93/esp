import machine
import time
led1 = machine.Pin(32, machine.Pin.OUT)
led2 = machine.Pin(33, machine.Pin.OUT)
while True:
    led1.value(0)
    led2.value(1)
    print(1)
    time.sleep(1)
    led1.value(1)
    led2.value(0)
    print(2)
    time.sleep(1)
