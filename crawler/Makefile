CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: clean crawler.bin crawler_1_5.bin

crawler.bin:
		gcc $(CFLAGS) crawler.c $(LIBS) -o $@

crawler_1_5.bin:
		gcc $(CFLAGS) crawler_1_5.c $(LIBS) -o $@

clean:
		rm -f *.o *.bin
