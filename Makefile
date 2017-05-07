CC = gcc
CFLAGS = -Wall -g

OBJS = connect4.o main.o


all: game server main

game: $(OBJS)

connect4.o: connect4.c
	$(CC) $(CFLAGS) -c connect4.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

server: server.c
	$(CC) $(CFLAGS) server.c -o server

main: connect4.c main.c
	$(CC) $(CFLAGS) $(OBJS) -o run

clean:
	rm -f *~ *.o run server
	rm -rf *.dSYM