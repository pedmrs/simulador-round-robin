CC=gcc
CFLAGS=-I. -Wall -g
DEPS = fila.h processo.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

simrr: main.o fila.o
	$(CC) -o simrr main.o fila.o
