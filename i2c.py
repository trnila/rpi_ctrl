import smbus
import time

BUS = 1
ADDR = 0x8

LEDS = [8, 16]

bus = smbus.SMBus(BUS)


def digitalWrite(pin, state):
    try:
        bus.write_byte_data(ADDR, pin, state)
    except IOError as e:
        print(e)


while True:
    for led in LEDS:
        digitalWrite(led, 1)
        time.sleep(1)
        digitalWrite(led, 0)
