#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include "jobs.h"
#include "common.h"

pid_t pidEnfant = 0;
struct sigaction oldAction;

void creeProcessusEnfant(char **argv,int argc){
  pid_t pid = fork();
  if(pid>0){ // Parent
    int status;
    wait(&status);
    int exit_status = WEXITSTATUS(status);
    printf("Foreground job exited with code %d\n",exit_status);
    return;
  }
  else if(pid == 0){ // Enfant
    char temp[100];
    memset(temp,0,100);
    sprintf(temp,"/bin/%s",argv[0]);
    execv(temp,argv);
    exit(EXIT_FAILURE);
  }
  else{ // Erreur
    printf("Fork failled\n");
    return;
  }
}

int detectBackground(char **argv,int argc){
  if(!strcmp(argv[argc-1],"&")){
    argv[argc-1] = NULL;
    return 1;
  }
  else{
    return 0;
  }
}

void creeProcessusEnfantBackground(char **argv,int argc){
  gestionSignal();
  pid_t pid = fork();
  if(pid>0){ // Parent
    pidEnfant = pid;
    sleep(1);
    return;
  }
  else if(pid == 0){ // Enfant
    char temp[100];
    memset(temp,0,100);
    sprintf(temp,"/bin/%s",argv[0]);
    if ((freopen("/dev/null", "w", stdout)) != NULL){
      execv(temp,argv);
      exit(EXIT_FAILURE);
    }
    else{
      die("La redirection a échoué");
    }
  }
  else{ // Erreur
    printf("Fork failled\n");
    return;
  }
}

static void handleEnd(int sig, siginfo_t *siginfo, void *context){
  int status;
  pid_t pid;
  pid = waitpid(pidEnfant,&status,0);
  int exit_status = WEXITSTATUS(status);
  printf("Background job %d exited with code %d\n",pid,exit_status);
  if (sigaction(SIGCHLD, &oldAction, NULL) < 0) {
    die("Sigaction");
  }
}

void gestionSignal(){
  struct sigaction act;
	memset (&act, '\0', sizeof(act));

  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = handleEnd;

  if (sigaction(SIGCHLD, &act, &oldAction) < 0) {
		die("Sigaction");
	}
}
