flags=-O2 -Wall -std=c2x
ldflags=-lbu

.PHONY: all clean

all: clean virtualMachine

virtualMachine: virtualMachine.o
	cc $(flags) $^ -o $@ $(ldflags)

virtualMachine.o: virtualMachine.c virtualMachine.h
	cc $(flags) -c $<

clean:
	rm -f *.o virtualMachine