.PHONY: all run clean

.PHONY: all run clean

all:
	mkdir -p build
	gcc -o build/server main.c libs/picohttpparser.c \
			-I ./libs/ \
	    -I ./libuv/include \
	    -L ./libuv/.libs -luv

run:
	DYLD_LIBRARY_PATH=./libuv/.libs ./build/server

clean:
	rm -f ./build/server

