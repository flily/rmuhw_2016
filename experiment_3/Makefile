CC=cc
CFLAGS=-Wall -g
BIN=contact

all: $(BIN)

$(BIN): contact.o
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY:
clean:
	rm -f $(BIN) *.o
