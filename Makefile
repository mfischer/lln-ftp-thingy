CC=gcc
CFLAGS=-Wall -Werror -std=c99 -D_POSIX_C_SOURCE
LDFLAGS=
EXEC=client server

all: $(EXEC)

client.o: client.c
	$(CC) -o $@ -c $< $(CFLAGS)

server.o: server.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm *.o
	rm $(EXEC)

mrproper: clean
