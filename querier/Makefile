CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: clean query.bin

query.bin:
		gcc $(CFLAGS) query.c $(LIBS) -o $@

clean:
		rm -f *.o *.bin
