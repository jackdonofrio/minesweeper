CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make

minesweeper: minesweeper.c
	$(CC) $(CFLAGS) $^ -o $@ -lcurses

.PHONY: clean

clean:
	rm -f *~ *.o *.out
	rm minesweeper