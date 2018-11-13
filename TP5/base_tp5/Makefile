CC=gcc
CFLAGS=-Wall -g --pedantic -std=c99

all: randClient randServer

common.o: common.c common.h
	$(CC) $(CFLAGS) -c -o common.o common.c

randServer.o: randServer.c common.h
	$(CC) $(CFLAGS) -c -o randServer.o randServer.c

randClient.o: randClient.c common.h
	$(CC) $(CFLAGS) -c -o randClient.o randClient.c

randServer: randServer.o common.o
	$(CC) $(CFLAGS) -o randServer randServer.o common.o

randClient: randClient.o common.o
	$(CC) $(CFLAGS) -o randClient randClient.o common.o

clean:
	rm -rf *.o randClient
