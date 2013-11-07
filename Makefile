CFLAGS=-Wall -O2

all: prg2bas bas2prg

prg2bas: prg2bas.o tokens.o
bas2prg: bas2prg.o tokens.o

.PHONY: clean
clean:
	rm -f prg2bas bas2prg *.o
