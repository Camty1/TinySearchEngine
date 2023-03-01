CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g -pthread
LIBS=-lutils -lcurl 

all: clean lcrawler.bin

lcrawler.bin:
		gcc $(CFLAGS) lcrawler.c $(LIBS) -o $@

clean:
		rm -f *.o *.bin
