import spidev
import time

BUS = 0
DEVICE = 1

PORT = 1 # PORTB
LED = 4

spi = spidev.SpiDev()
spi.open(BUS, DEVICE)
spi.max_speed_hz = 100000

#spi.xfer([0xDE, 0xAD, 0xBE, 0xAF])
while True:
    delay = 1

    while delay < 8:
        spi.xfer([PORT, 1 << LED, 0xFF])
        time.sleep(1.0 / delay)
        spi.xfer([PORT, 1 << LED, 0x00])
        time.sleep(1.0 / delay)
        print(1.0 / delay)

        delay += 1
