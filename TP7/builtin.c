#include "jobs.h"
#include "builtin.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void determineCommande(char **argv,int argc){
  if(!strcmp(argv[0],"cd")){
    commandeCd(argv[1]);
    return;
  }
  else if(!strcmp(argv[0],"exit")){
    commandExit(argv,argc);
    return;
  }
  else{
    if(detectBackground(argv,argc)){
      creeProcessusEnfantBackground(argv,argc);
    }
    else{
      creeProcessusEnfant(argv,argc);
    }
  }
}

void commandExit(char **argv,int argc){
  freeTab(argv,argc);
  exit(EXIT_SUCCESS);
}

void commandeCd(const char *path){
  chdir(path);
}
