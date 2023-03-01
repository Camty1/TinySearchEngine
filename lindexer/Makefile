CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g -pthread
LIBS=-lutils -lcurl 

all: clean lindexer.bin

lindexer.bin:
		gcc $(CFLAGS) lindexer.c $(LIBS) -o $@

clean:
		rm -f *.o *.bin
