#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

int buildArgv(char **argv,char commande[]){
  char *token;
  int argc=0;

  commande[strlen(commande)-1] = ' ';

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

void freeTab(char **argv,int argc){
  for(int k=0;k<argc;k++){
    free(argv[k]);
  }
}
