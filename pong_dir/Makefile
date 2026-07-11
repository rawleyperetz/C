
CC=gcc
CFLAGS=-Wall -Wextra -g

pong: pong.c
	$(CC) $(CFLAGS) pong.c -o pong $(shell sdl2-config --cflags --libs) -lSDL2_ttf

clean:
	rm -f pong

run:
	./pong
