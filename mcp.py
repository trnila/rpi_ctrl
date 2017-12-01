import smbus

ADDR = 0x20

IODIRA = 0x00
IODIRB = 0x01
# outputs
OLATA = 0x14
OLATB = 0x15
# inputs
GPIOA = 0x12
GPIOB = 0x13
# pull ups
GPPUA = 0x0c
GPPUB = 0x0d


class MCP23017:
    def __init__(self, bus, addr = ADDR):
        self.addr = addr
        self.bus = smbus.SMBus(bus)
        self.banks = [GPIOA, GPIOB]

    def write(self, bank, pin, state):
        while True:
            try:
                data = self.bus.read_byte_data(self.addr, self.banks[bank])
                if state:
                    data |= 1 << pin
                else:
                    data &= ~(1 << pin)
                self.bus.write_byte_data(self.addr, self.banks[bank], data) 
                return
            except IOError as e:
                pass

        raise e

    def read(self, bank, pin):
        pass


import time
mcp = MCP23017(1)
state = 1
MAX = 4
while True:
    for i in range(0, MAX):
        pin = i
        if not state:
            pin = MAX - pin

        mcp.write(0, pin, state)
        time.sleep(0.1)
    state = not state

