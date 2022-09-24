import machine
from time import sleep
from ulora import LoRa, ModemConfig, SPIConfig

led1 = machine.Pin(21, machine.Pin.OUT)
led1_val = 0
led1.value(led1_val)

# This is our callback function that runs when a message is received
def on_recv(payload):
    led1_val = 0 if led1_val == 1 else 1 
    led1.value(led1_val)
    print("From:", payload.header_from)
    print("Received:", payload.message)
    print("RSSI: {}; SNR: {}".format(payload.rssi, payload.snr))

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
lora = LoRa(RFM95_SPIBUS, RFM95_INT, SERVER_ADDRESS, RFM95_CS, 
            reset_pin=RFM95_RST, freq=RF95_FREQ, tx_power=RF95_POW, acks=True,
            receive_all=True)

# set callback
lora.on_recv = on_recv

# set to listen continuously
lora.set_mode_rx()

# loop and wait for data
while True:
    sleep(0.1)