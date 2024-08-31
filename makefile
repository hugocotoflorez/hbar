CC = gcc -Wall
SRC = *.c
OUT = hbar
LIB = -L. -ltermstuff

all:
	$(CC) $(SRC) -o $(OUT) -g $(LIB)
