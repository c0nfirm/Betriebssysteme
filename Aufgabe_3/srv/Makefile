#!/usr/bin/make
.SUFFIXES:
TAR = server
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)

CFLAGS = -std=gnu11 -c -g -Os -Wall -Werror

%.o: %.c %.h
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

server: $(OBJ)
	$(CC) $(LFLAGS) $^ -o $@

all: $(TAR)

clean:
	$(RM) $(RMFILES) $(OBJ) $(TAR)
