all: messages.pb.c

%.pb.c: %.pb
	python ~/nanopb/generator/nanopb_generator.py $^

%.pb: %.proto
	protoc $^ -o $@

clean:
	rm -f *.pb *.pb.c *.pb.h
