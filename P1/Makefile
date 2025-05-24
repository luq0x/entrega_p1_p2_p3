CC      = gcc
CFLAGS  = -Wall -O2

.PHONY: all run clean

all: compiler assembler executor

compiler: compiler.c neander.h
	$(CC) $(CFLAGS) -o $@ $<

assembler: assembler.c neander.h
	$(CC) $(CFLAGS) -o $@ $<

executor: executor.c neander.h
	$(CC) $(CFLAGS) -o $@ $<

run: programa.lpn 
	./compiler programa.lpn
	./assembler programa.asm
	./executor programa.bin

clean:
	rm -f compiler assembler executor programa.asm programa.bin
