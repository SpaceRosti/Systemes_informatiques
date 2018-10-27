ultra-cp: main.o listeFichier.o backup.o
	gcc -g main.o listeFichier.o backup.o -o ultra-cp

main.o: main.c listeFichier.h backup.h
	gcc -g -c main.c -o main.o

backup.o: backup.c
	gcc -g -c backup.c -o backup.o

listeFichier.o: listeFichier.c
	gcc -g -c listeFichier.c -o listeFichier.o
