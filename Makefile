CXXFLAGS=-I./vendor/nanopb
CFLAGS=$(CXXFLAGS)
CC=g++

OBJECTS=main.o \
	serial.o \
	Messages/messages.pb.o \
	vendor/nanopb/pb_common.o \
	vendor/nanopb/pb_decode.o \
	vendor/nanopb/pb_encode.o
	

main: $(OBJECTS)
