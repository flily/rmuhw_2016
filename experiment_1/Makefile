CC=gcc
CFLAGS=-g -Wall

all: server  client

server: server.o
	$(CC) -o $@ $<

client: client.o
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) $<

.PHONY:
clean:
	rm -f *.o server client
