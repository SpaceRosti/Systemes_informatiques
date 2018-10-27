#ifndef BACKUP_H_INCLUDED
#define BACKUP_H_INCLUDED

#include <stdio.h>

void copieFichier(char *source,char *destination,int argA);
void copieMultiple(int argc,char *argv[],int argA);
void copieDeuxFichier(char *source,char *destination,int cas,int argA);
void gereRepertoire(char *source,char *destination,int argA);
int modifCheck(char *source, char *destination);
void gestionPerm(char *source,char *destination);
int gestionArg(int *argc,char *argv[]);

#endif
