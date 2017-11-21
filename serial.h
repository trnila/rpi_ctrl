#pragma once

#include "serial.h"
#include <termios.h>

class Serial {
public:
	Serial(const char* path, int baud);
	~Serial();

	void send(int type, int size, void *data);

private:
	int fd;

	int setInterfaceAttribs(int speed, int parity);
	void setBlocking(int should_block);
};

