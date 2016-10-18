CC=gcc
CFLAGS= -ansi -Wall -g -DDEBUG -pedantic -pthread

CFILES=client.c
HFILES=
OFILES=client.o

all:	client

%.o:	%.c $(HFILES)
	$(CC) -c $(CFLAGS) $< -o $@

client:	$(OFILES) $(HFILES)
	$(CC) $(CFLAGS) $(OFILES) -o client


clean:
	rm -f *~ client client.o 
