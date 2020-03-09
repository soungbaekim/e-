
CC=gcc
CFLAGS=-I./src

LIBS=-lcurl -lpthread
BDIR=./build

all: clean build build_debug

build: dir build/status.o build/main.o build/base64.o
	$(CC) build/status.o build/main.o build/base64.o -o build/output $(LIBS)

build_debug: build/status_DEBUG.o build/main_DEBUG.o build/base64.o
	$(CC) build/status_DEBUG.o build/main_DEBUG.o build/base64.o -o build/output_DEBUG $(LIBS)

build/status_DEBUG.o: src/status.c
	$(CC) $(CFLAGS) -D DEBUG -c $^ -o $@

build/main_DEBUG.o: src/main.c
	$(CC) $(CFLAGS) -D DEBUG -c $^ -o $@

build/status.o: src/status.c
	$(CC) $(CFLAGS) -c $^ -o $@

build/main.o: src/main.c
	$(CC) $(CFLAGS) -c $^ -o $@

build/base64.o: src/base64.c
	$(CC) $(CFLAGS) -c $^ -o $@

dir:
	mkdir build

run: build/output
	build/output

test: build/output
	build/output < res/uuids.txt

debug: build/output_DEBUG
	build/output_DEBUG

.PHONY: clean run make

clean:
	rm -rf ./build
