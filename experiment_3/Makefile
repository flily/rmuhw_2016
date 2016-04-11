CC=cc
CFLAGS=-Wall -g -std=c89
BIN=contact

all: $(BIN)

$(BIN): contact.o
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

.PHONY:
clean:
	rm -f $(BIN) *.o
