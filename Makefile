CC=gcc
CFLAGS=-Wall -Werror -std=c99 -D_POSIX_C_SOURCE -g
LDFLAGS=-ledit
EXEC=client server

.PHONY: all
all: $(EXEC)

client: client.o utils.o handlers.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

server: server.o utils.o handlers.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

client.o: client.c utils.h constants.h
	$(CC) $(CFLAGS) -o $@ -c $<

server.o: server.c utils.h constants.h
	$(CC) $(CFLAGS) -o $@ -c $<

utils.o: utils.c utils.h constants.h
	$(CC) $(CFLAGS) -o $@ -c $<

handlers.o: handlers.c utils.h constants.h
	$(CC) $(CFLAGS) -o $@ -c $<


.PHONY: clean
clean:
	rm *.o
	rm $(EXEC)

.PHONY: mrproper
mrproper: clean
