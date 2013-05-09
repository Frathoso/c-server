CC=gcc
CFLAGS=-Wall -pthread -lpthread -std=c99 -D_GNU_SOURCE
TARGET=server

all:
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET) 

clean:
	rm -f *.o
