from machine import Pin, PWM
import time

servo = PWM(Pin(23), freq=50)
while(True):
    servo.duty(30)
    time.sleep(2)
    servo.duty(80)
    time.sleep(2)
    