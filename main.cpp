#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include "pb.h"
#include "pb_encode.h"
#include "Messages/messages.pb.h"
#include "serial.h"


#define registerMsg(id, T) \
void sendMsg(Serial &serial, T &msg) { \
	uint8_t buffer[T##_size]; \
	pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); \
	if(!pb_encode(&stream, T##_fields, &msg)) { \
		throw std::runtime_error("Failed to encode message"); \
	} \
\
	serial.send(id, stream.bytes_written, (void*) buffer); \
}

registerMsg(0, GpioControlMsg);
registerMsg(1, ReverseMsg);
registerMsg(2, PingMsg);

void digital_write(Serial &serial, _GpioControlMsg_Port port, int pin, int state) {
	GpioControlMsg msg;
	msg.port = port;
	msg.pin = pin;
	msg.state = state;

	sendMsg(serial, msg);
}

int main(int argc, char** argv) {
	if(argc != 3) {
		printf("Usage: %s string number\n", argv[0]);
		return 1;
	}

	Serial serial("/dev/ttyAMA0", B921600);
	serial.addHandler(2, PingMsg_fields, [](void *param) {
		PingMsg *msg = (PingMsg*) param;

		printf("Received ping: %d\n", msg->seq);
	});

	int seq = 0;
	for(;;) {
		ReverseMsg msg;
		strcpy(msg.name, "ahoj");
		msg.num = 18;

		//sendMsg(serial, msg);


		if(rand() % 5 == 0) {
			PingMsg ping;
			ping.seq = seq++;
			sendMsg(serial, ping);
			usleep(100 * 1000);
		}


		digital_write(serial, GpioControlMsg_Port_PortB, 4, rand() % 2);
		usleep(100 * 1000);
	}

	return 0;
}
