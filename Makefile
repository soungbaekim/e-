
CC=gcc
CFLAGS=-I./src -I./lib/include

LIBS=-lcurl -lpthread
BDIR=./build


default: build build/status.o build/main.o build/base64.o
	$(CC) build/status.o build/main.o build/base64.o -o build/output $(LIBS)

build/status.o: src/status.c
	$(CC) $(CFLAGS) -c src/status.c -o $@

build/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o $@

build/base64.o: src/base64.c
	$(CC) $(CFLAGS) -c $^ -o $@

build:
	mkdir build

run: build/output
	build/output

.PHONY: clean run make

clean:
	rm -rf ./build
