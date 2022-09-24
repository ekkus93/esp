import machine

p2 = machine.Pin(2)
servo = machine.PWM(p2,freq=50)

print("setting servo")
# duty for servo is between 40 - 115
servo.duty(100)