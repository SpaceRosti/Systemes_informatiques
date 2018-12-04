#include "jobs.h"
#include "builtin.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// variable globale pour retenir le pid d'un processus en background
pid_t backgroundPid = 0;

// Exécute la commande passée en paramètre(argv)
void determineCommande(char **argv,int argc){
  if(argv[0]==NULL){// si aucune commande n'est passé en paramètre
    return;
  }
  if(!strcmp(argv[0],"cd")){ // cd builtin
    commandeCd(argv[1]);
    return;
  }
  else if(!strcmp(argv[0],"exit")){// exit builtin
    commandExit(argv,argc);
    return;
  }
  else if(argv[0]!=NULL){
    // detecte si le processus doit être lancé en background
    if(detectBackground(argv,argc)){
      creeProcessusEnfantBackground(argv,argc); // cree processus en background
    }
    else{
      creeProcessusEnfant(argv,argc); // cree processus en frontground
    }
  }
}

void commandExit(char **argv,int argc){ // quitte le shell
  if(backgroundPid != 0){
    // tue les enfants
    kill(backgroundPid,SIGKILL);
  }
  // libere la mémoire
  freeTab(argv,argc);
  // quitte
  exit(EXIT_SUCCESS);
}

void commandeCd(const char *path){ // change le répertoire courant
  chdir(path);
}
