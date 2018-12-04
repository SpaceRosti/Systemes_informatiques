#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "common.h"
#include "shell.h"
#include "builtin.h"
#include "jobs.h"
#include <sys/stat.h>
#include <sys/types.h>

#define MAXCOMMANDE 1000
#define MAXARGV 256

int main(void){
  char commande[MAXCOMMANDE];
  int argc;
  char **argv=NULL;

  // Remplace la gestion des signaux par défaut
  // par la gestion demandée dans l'énnoncé
  gestionDesSignaux();

  while(1){
    // reçoit la commande à exectuer
    if(fgets(commande,MAXCOMMANDE,stdin)<0){
      die("Impossible de lire la commande");
    }

    argv = malloc(sizeof(char *) * MAXARGV);
    // construit le tableau argc et renvoie argc
    argc = buildArgv(argv,commande);

    // exectue la commande passé plus haut
    determineCommande(argv,argc);

    // libère la mémoire
    freeTab(argv,argc);
  }
}
