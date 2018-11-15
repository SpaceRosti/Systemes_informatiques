client: main.o common.o
	gcc main.o common.o -o client

main.o: main.c common.h
	gcc -c main.c -o main.o

common.o: common.c
	gcc -c common.c -o common.o
