#pragma once

#include <termios.h>
#include <thread>
#include <unordered_map>
#include "serial.h"


class Serial {
public:
	typedef std::function<void(void*)> Callback;

	Serial(const char* path, int baud);
	~Serial();

	void send(int type, int size, void *data);
	void addHandler(int type, const pb_field_t *fields, Callback cb) {
		handlers[type] = FrameHandler{
				.fields = fields,
				.callback = cb
		};
	}

private:
	struct FrameHandler {
		const pb_field_t *fields;
		Callback callback;
	};

	int fd;

	std::thread thread;
	std::unordered_map<int, FrameHandler> handlers;

	int setInterfaceAttribs(int speed, int parity);
	void setBlocking(int should_block);
};

