CC = gcc
CFLAGS = -Wall -g

OBJS = connect4.o main.o


all: game server main client

game: $(OBJS)

connect4.o: connect4.c
	$(CC) $(CFLAGS) -c connect4.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

server: server.c
	$(CC) $(CFLAGS) server.c -o server
	
client: Test.c
	$(CC) $(CFLAGS) Test.c connect4.c -o client

main: connect4.c main.c
	$(CC) $(CFLAGS) $(OBJS) -o run

clean:
	rm -f *~ *.o run server client
	rm -rf *.dSYM
