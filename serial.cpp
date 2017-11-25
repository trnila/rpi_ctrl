#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include "pb.h"
#include "pb_decode.h"
#include "serial.h"
#include "Messages/messages.pb.h"

const int frameSize = 20;


Serial::Serial(const char* path, int baud) {
	fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		throw std::runtime_error("could not open serial");
	}

	setInterfaceAttribs(baud, 0);
	setBlocking(0);

	thread = std::thread([=]() {
		uint8_t header[3];
		uint8_t payload[1024];
		uint8_t msg[1024];

		for(;;) {
			memset(header, 0, sizeof(header));
			memset(payload, 0, sizeof(payload));
			memset(msg, 0, sizeof(msg));

			if(read(fd, header, sizeof(header)) != sizeof(header)) {
				printf("failed to receive header\n");
				continue;
			}
			printf("header: %x %x %x\n", header[0], header[1], header[2]);

			int packetSize = (header[1] << 8) | header[2];
			printf("packet %d, size: %d\n", header[0], packetSize);

			if(read(fd, payload, packetSize) != packetSize) {
				throw std::runtime_error("failed to received payload");
			}

			auto it = handlers.find(header[0]);
			if(it != handlers.end()) {
				auto stream = pb_istream_from_buffer(payload, packetSize);
				if (!pb_decode(&stream, it->second.fields, &msg)) {
					throw std::runtime_error("failed to decode ack packet");
				}

				it->second.callback(msg);
			} else {
				printf("Unknown frame\n");
			}
		}
	});
}

Serial::~Serial() {
	close(fd);
}

int Serial::setInterfaceAttribs(int speed, int parity) {
	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0) {
		throw std::runtime_error("tcgetattr failed");
	}

	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
	// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 50;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
	// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr (fd, TCSANOW, &tty) != 0) {
		throw std::runtime_error("tcsetattr failed");
	}
	return 0;
}

void Serial::setBlocking(int should_block) {
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0) {
		throw std::runtime_error("tcgetattr failed");
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 50;            // 0.5 seconds read timeout

	if (tcsetattr (fd, TCSANOW, &tty) != 0) {
		throw std::runtime_error("tcsetattr failed");
	}
}

void Serial::send(int type, int size, void *data) {
	uint8_t header[3];
	header[0] = type;
	header[1] = (size & 0xFF00) >> 8;
	header[2] = size & 0xFF;

	write(fd, header, sizeof(header));
	write(fd, data, size);
	for(int i = sizeof(header) + size; i < frameSize; i++) {
		char c = 0;
		write(fd, &c, 1);
	}
}
