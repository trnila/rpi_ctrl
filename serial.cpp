#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include "serial.h"

const char ACK_BYTE = 'x';

Serial::Serial(const char* path, int baud) {
	fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		throw std::runtime_error("could not open serial");
	}

	setInterfaceAttribs(baud, 0);
	setBlocking(0);
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
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

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
	write(fd, &type, sizeof(type));
	write(fd, &size, sizeof(size));
	write(fd, data, size);

	char ack = 0;
	read(fd, &ack, 1);
	if(ack != ACK_BYTE) {
		throw std::runtime_error("Did not received ACK byte");
	}
}
