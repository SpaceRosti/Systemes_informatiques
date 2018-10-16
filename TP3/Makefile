HashFun: main.o lectureArgument.o hashage.o
	gcc main.o lectureArgument.o hashage.o -o HashFun -l ssl -l crypto

main.o: main.c lectureArgument.h hashage.h
	gcc -c main.c -o main.o

lectureArgument.o: lectureArgument.c lectureArgument.h hashage.h
	gcc -c lectureArgument.c -o lectureArgument.o

hashage.o: hashage.c hashage.h
	gcc -c hashage.c -o hashage.o -l ssl -l crypto
