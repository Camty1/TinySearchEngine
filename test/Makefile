CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g                         
LIBS=-lutils -lcurl

all: clean pageio_test.bin

pageio_test.bin:
	gcc $(CFLAGS) pageio_test.c $(LIBS) -o $@

clean:
	rm -f *.o *.bin          
