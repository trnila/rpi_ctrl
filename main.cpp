#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pb.h"
#include "pb_encode.h"
#include "Messages/messages.pb.h"
#include "serial.h"


void digital_write(Serial &serial) {
	GpioControlMsg msg;
	msg.port = GpioControlMsg_Port_PortB;
	msg.pin = 4;
//	msg.state = rand() % 2;
	msg.state = 1;

	uint8_t buffer[128];
	pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
	if(!pb_encode(&stream, GpioControlMsg_fields, &msg)) {
		printf("Failed");
	}

	printf("=%d\n", stream.bytes_written);
	for(int i = 0; i <stream.bytes_written; i++) {
		printf("%X ", buffer[i]);
	}
	serial.send(100, stream.bytes_written, (void*) buffer);
}

int main(int argc, char** argv) {
	Serial serial("/dev/ttyAMA0", B921600); 

	if(argc != 3) {
		printf("Usage: %s string number\n", argv[0]);
		return 1;
	}

	char buf[100];
	int len = 0;


	ReverseMsg msg = ReverseMsg_init_zero;
	strcpy(msg.name, argv[1]);
	msg.num = atoi(argv[2]);


	uint8_t buffer[128];
	pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
	if(!pb_encode(&stream, ReverseMsg_fields, &msg)) {
		printf("Failed");
	}


	printf("%d\n", stream.bytes_written);
	for(;;) {
		digital_write(serial);
		usleep(100 * 1000);
	}
	return 1;
	/*time_t start = time(NULL);
	int i = 0;
	for(;;) {
		send_data(fd, 1234678, stream.bytes_written, buffer);
		char c;
		read(fd, &c, 1);
		if(c != 'x') {
			printf("Got: %d %c\n", c, c);
			return -1;
		}
		i++;
		if(i % 100 == 0) {
			int elapsed = time(NULL) - start;
			if(elapsed != 0) {
				printf("%d\n", i / elapsed);
			}
		}
	}*/
}
