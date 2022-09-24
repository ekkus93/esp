# Load the machine module for GPIO and PWM
# Control servo motor with MicroPython
# Author: George Bantique, TechToTinker
# Date: September 15, 2020

import machine
# Load the time module for the delays
import time

# Create a regular p23 GPIO object
p2 = machine.Pin(2, machine.Pin.OUT)

# Create another object named pwm by
# attaching the pwm driver to the pin
pwm = machine.PWM(p2)

# Set the pulse every 20ms
pwm.freq(50)

# Set initial duty to 0
# to turn off the pulse
pwm.duty(0)

# Creates a function for mapping the 0 to 180 degrees
# to 20 to 120 pwm duty values
def map(x, in_min, in_max, out_min, out_max):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

# Creates another function for turning 
# the servo according to input angle
def servo(pin, angle):
    pin.duty(map(angle, 0, 180, 20, 120))
    
print(1)
servo(pwm, 0)
print(2)
servo(pwm, 90)
print(3)
servo(pwm, 180)
print(4)
pwm.duty(0)


pwm.deinit()