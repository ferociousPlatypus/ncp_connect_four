CC = gcc
CFLAGS = -Wall -g

OBJS = connect4.o main.o

all: game
	$(CC) $(CFLAGS) $(OBJS) -o run

game: $(OBJS)

connect4.o: connect4.c
	$(CC) $(CFLAGS) -c connect4.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *~ *.o run
