CC=gcc
CFLAGS=-W -Wall -ansi -pedantic
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
