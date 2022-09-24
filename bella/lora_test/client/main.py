import machine
from time import sleep
from ulora import LoRa, ModemConfig, SPIConfig

# Lora Parameters
RFM95_RST = 14
RFM95_SPIBUS = (1, 18, 23, 19)
RFM95_CS = 5
RFM95_INT = 2
RF95_FREQ = 915.0
RF95_POW = 23
CLIENT_ADDRESS = 1
SERVER_ADDRESS = 2

# initialise radio
lora = LoRa(RFM95_SPIBUS, RFM95_INT, CLIENT_ADDRESS, RFM95_CS,
            reset_pin=RFM95_RST, freq=RF95_FREQ, tx_power=RF95_POW, acks=False, receive_all=True)

led1 = machine.Pin(21, machine.Pin.OUT)
led1.value(0)

# loop and send data
while True:
    led1.value(1)
    lora.send_to_wait("This is a test message", SERVER_ADDRESS)
    print("sent")
    sleep(1)
    led1.value(0)
    sleep(10)