ultra-cp: main.o listeFichier.o backup.o
	gcc main.o listeFichier.o backup.o -o ultra-cp

main.o: main.c listeFichier.h backup.h
	gcc -c main.c -o main.o

backup.o: backup.c
	gcc -c backup.c -o backup.o

listeFichier.o: listeFichier.c
	gcc -c listeFichier.c -o listeFichier.o
