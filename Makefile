CC=gcc
CFLAGS=-Wall -Werror -std=c99 -pedantic
LDFLAGS=
EXEC=client server

all: $(EXEC)

client: client.o
	$(CC) -o $@ $^ $(LDFLAGS)

client.o: client.c
	$(CC) -o $@ -c $< $(CFLAGS)

server.o: server.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm *.o
	rm $(EXEC)

mrproper: clean
