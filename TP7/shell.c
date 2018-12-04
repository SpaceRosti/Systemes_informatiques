#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "common.h"
#include "shell.h"

extern pid_t frontPid;
extern pid_t backgroundPid;

// construit argv et renvoie argc
int buildArgv(char **argv,char commande[]){
  char *token;
  int argc=0;

  // remplace \0 par un espace
  commande[strlen(commande)-1] = ' ';

  // parse la chaine de commande pour remplir les cases de argv
  token = strtok(commande," ");
  while(token != NULL){
    if(strcmp(token,"\n")){
      argv[argc] = malloc(sizeof(char) *(1+strlen(token)));
      strcpy(argv[argc],token);
      argc++;
    }
    token = strtok(NULL," ");
  }
  return argc;
}

// libère la mémoire des cases de argv
void freeTab(char **argv,int argc){
  for(int k=0;k<argc;k++){
    free(argv[k]);
  }
}

// handler de SIGINT
void sigintHandler(int sig){
  if(frontPid != 0){
    kill(frontPid,SIGKILL);
  }
}

// handler de SIGHUP
void sighupHandler(int sig){
  if(backgroundPid != 0){
    kill(backgroundPid,SIGKILL);
  }
  if(frontPid != 0){
    kill(frontPid,SIGKILL);
  }
  exit(EXIT_SUCCESS);
}

// change la gestion des signaux par défaut par celle demandé dans l'énnoncé
void gestionDesSignaux(){
  //ignore SIGQUIT et SIGTERM
  if (signal(SIGQUIT, SIG_IGN) < 0) {
		die("Signal");
	}
  if (signal(SIGTERM, SIG_IGN) < 0) {
		die("Signal");
	}
//----------------------------------------------------------------------------
// SIGINT
  struct sigaction act;
  memset (&act, '\0', sizeof(act));

  act.sa_handler = sigintHandler;
  act.sa_flags = SA_RESTART;

  if (sigaction(SIGINT, &act, NULL) < 0) {
    die("Sigaction");
  }
//----------------------------------------------------------------------------
// SIGHUP
  struct sigaction act2;
  memset (&act2, '\0', sizeof(act2));

  act2.sa_handler = sighupHandler;
  act.sa_flags = SA_RESTART;

  if (sigaction(SIGHUP, &act2, NULL) < 0) {
    die("Sigaction");
  }

}
