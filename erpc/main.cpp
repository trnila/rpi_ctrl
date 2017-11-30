#include <stdio.h>
#include <unistd.h>
#include <array>
#include <termios.h>
#include "erpc_client_setup.h"
#include "messages.h"

const auto PORT = GPIO_B;
const std::array<int, 3> LEDS{0, 3, 4};
const char *SERIAL_PATH = "/dev/ttyAMA0";
const int BAUD_RATE = B921600;

int main() {
	erpc_transport_t transport = erpc_transport_serial_init(SERIAL_PATH, BAUD_RATE);
	erpc_mbf_t message_buffer_factory = erpc_mbf_dynamic_init();
	erpc_client_init(transport, message_buffer_factory);

	bool state = 1;
	for(;;) {
		for(const int led: LEDS) {
			printf("set_led(PORTB, %d, %d) = ", 1 << led, state);
			printf("%d\n", set_led(PORT, 1 << led, state));

			usleep(100 * 1000);
		}
		state = !state;
	}
}