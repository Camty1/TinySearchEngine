CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: clean crawler.bin

crawler.bin:
		gcc $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
		rm -f *.o *.bin
