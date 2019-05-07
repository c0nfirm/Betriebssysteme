#!/usr/bin/make
.SUFFIXES:
.PHONY: all run pack clean
SRC = shell.c prog.c array.c
TAR = shell prog
OBJ = $(SRC:%.c=%.o)
PCK = lab-2.zip

CFLAGS = -std=gnu11 -c -g -Os -Wall -Werror

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%: %.o
	$(CC) -o $@ $^

shell: shell.o array.o

all: $(TAR)

run: $(TAR)
	./shell

pack:
	zip $(PCK) $(SRC) $(wildcard *.h) Makefile

clean:
	$(RM) $(RMFILES) $(OBJ) $(TAR) $(PCK)
