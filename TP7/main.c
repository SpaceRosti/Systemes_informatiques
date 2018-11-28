#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "shell.h"
#include "builtin.h"
#include <sys/stat.h>
#include <sys/types.h>

#define MAXCOMMANDE 1000
#define MAXARGV 256

int main(void){
  char commande[MAXCOMMANDE];
  int argc;
  char **argv=NULL;

  while(1){
    printf(">");
    if(fgets(commande,MAXCOMMANDE,stdin)<0){
      die("Impossible de lire la commande");
    }

    argv = malloc(sizeof(char *) * MAXARGV);
    argc = buildArgv(argv,commande);

    determineCommande(argv,argc);

    freeTab(argv,argc);
  }
}
