CC = gcc
SRC = $(wildcard src/*.c)
CFLAGS ?= $(shell pkg-config --cflags raylib)
LIBS ?= $(shell pkg-config --libs raylib) -lm

all:
	$(CC) $(SRC) -o src/jogo $(CFLAGS) $(LIBS)

run: all
	cd src && ./jogo

clean:
	rm -f src/jogo