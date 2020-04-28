CFLAGS=-std=c99 -g -pedantic-errors -lrt -lsocket -lnsl -Wall -pedantic
all: main

clean:
	rm *.o main

main: main.c log.c
		gcc $(CFLAGS) -c main.c log.c
		gcc -g -o main main.o log.o -no-pie
