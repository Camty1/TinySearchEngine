CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-Iutils -Icurl

crawler:
		gcc $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
		rm -f *.o test_*.bin
