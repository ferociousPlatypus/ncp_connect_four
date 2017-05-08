CC = gcc
CFLAGS = -Wall -g

OBJS = server client

all: game

game: $(OBJS)

server: server.c
	$(CC) $(CFLAGS) server.c -o server
	
client: client.c
	$(CC) $(CFLAGS) connect4.c client.c -o client

clean:
	rm -f *~ *.o server client
	rm -rf *.dSYM
