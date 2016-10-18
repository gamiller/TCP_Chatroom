CC=gcc
CFLAGS= -ansi -Wall -g -DDEBUG -pedantic -pthread

CFILES=server.c queue.c lqueue.c
HFILES= queue.h lqueue.h
OFILES=server.o queue.o lqueue.o

all:	server

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

server:	$(OFILES) $(HFILES)
	$(CC) $(CFLAGS) $(OFILES) -o server


clean:
	rm -f *~ server server.o queue.o lqueue.o
