CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: clean indexer.bin

indexer.bin:
		gcc $(CFLAGS) indexer.c $(LIBS) -o $@

clean:
		rm -f *.o *.bin
