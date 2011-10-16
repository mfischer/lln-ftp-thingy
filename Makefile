CC=gcc
CFLAGS=-Wall -Werror -std=c99 -D_POSIX_C_SOURCE -g
LDFLAGS=-ledit -lhistory
#-ledit
EXEC=myftp myftpd

.PHONY: all
all: $(EXEC)

myftp: client.o utils.o handlers.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

myftpd: server.o utils.o handlers.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

client.o: client.c utils.h constants.h
	$(CC) $(CFLAGS) -o $@ -c $<

server.o: server.c utils.h constants.h
	$(CC) $(CFLAGS) -o $@ -c $<

utils.o: utils.c utils.h constants.h
	$(CC) $(CFLAGS) -o $@ -c $<

handlers.o: handlers.c utils.h constants.h
	$(CC) $(CFLAGS) -o $@ -c $<

doc: utils.h constants.h handlers.h client.c server.c
	doxygen $^

.PHONY: clean
clean:
	clear	
	rm *.o
	rm $(EXEC)
	rm -r html latex

.PHONY: mrproper
mrproper: clean
