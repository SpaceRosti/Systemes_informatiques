shell: main.o shell.o common.o builtin.o jobs.o
	gcc main.o shell.o common.o builtin.o jobs.o -o shell

main.o: main.c common.h builtin.h shell.h jobs.h
	gcc -Wall -c main.c -o main.o

shell.o: shell.c common.h
	gcc -Wall -c shell.c -o shell.o

builtin.o: builtin.c shell.h jobs.h
	gcc -Wall -c builtin.c -o builtin.o

jobs.o: jobs.c common.h
	gcc -Wall -c jobs.c -o jobs.o

common.o: common.c
	gcc -Wall -c common.c -o common.o

clean:
	rm -rf *.o shell
