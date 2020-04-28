CFLAGS=-std=c99 -g -pedantic-errors -lrt  -Wall -pedantic
all: main

clean:
	rm *.o main

main: main.c log.c bank_robbery.c
		gcc $(CFLAGS) -c main.c log.c bank_robbery.c
		gcc -g -o pa2 main.o log.o bank_robbery.o