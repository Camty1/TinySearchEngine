CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g -pthread
LIBS=-lutils -lcurl 

all: clean pageio_test.bin test_lqueue.bin lhash_test.bin

pageio_test.bin:
	gcc $(CFLAGS) pageio_test.c $(LIBS) -o $@

test_lqueue.bin:
	gcc $(CFLAGS) test_lqueue.c $(LIBS) -o $@

lhash_test.bin:
	gcc $(CFLAGS) lhash_test.c $(LIBS) -o $@

clean:
	rm -f *.o *.bin          
