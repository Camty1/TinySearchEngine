CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-Iutils -Icurl

all: crawler

crawler:
		gcc $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
		rm -f *.o crawler
