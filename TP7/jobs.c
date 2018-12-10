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

// pour retenir les anciennes sigaction
struct sigaction oldAction;
struct sigaction oldSIGINT;

extern pid_t backgroundPid;
pid_t frontPid;

// cree processus enfant en forground
void creeProcessusEnfant(char **argv,int argc){
  pid_t pid = fork();
  if(pid>0){ // Parent
    frontPid = pid;
    int status;
    wait(&status);
    int exit_status = WEXITSTATUS(status);
    printf("Foreground job exited with code %d\n",exit_status);
    return;
  }
  else if(pid == 0){ // Enfant
    execvp(argv[0],argv);
    // si la commande ne peut pas être executée,
    // quitte le processus avec une erreur
    exit(EXIT_FAILURE);
  }
  else{ // Erreur
    printf("Fork failled\n");
    return;
  }
}

// controle si le symbole "&" est situé à la fin de la commande passé en
// paramètre(argv).
int detectBackground(char **argv,int argc){
  if(!strcmp(argv[argc-1],"&")){
    argv[argc-1] = NULL;
    return 1;
  }
  else{
    return 0;
  }
}

// cree un processus enfant en backgroun
void creeProcessusEnfantBackground(char **argv,int argc){
  gestionSignal();// change la gestion du signal SIGCHLD
  pid_t pid = fork();
  if(pid>0){ // Parent
    backgroundPid = pid; // stocke le pid de l'enfant
    return;
  }
  else if(pid == 0){ // Enfant
    // redirige stdout vers /dev/null
    if ((freopen("/dev/null", "w", stdout)) != NULL){
      execvp(argv[0],argv);
      // si la commande ne peut pas être executée,
      // quitte le processus avec une erreur
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

// handler de SIGCHLD
void handleEnd(int sig, siginfo_t *siginfo, void *context){
  int status;
  pid_t pid;
  // supprime les traces de l'enfant une fois que celui-ci est mort
  pid = waitpid(backgroundPid,&status,0);
  int exit_status = WEXITSTATUS(status);
  printf("Background job %d exited with code %d\n",pid,exit_status);
  if (sigaction(SIGCHLD, &oldAction, NULL) < 0) {
    die("Sigaction");
  }
}

// change la gestion du signal SIGCHLD
void gestionSignal(){
  struct sigaction act;
	memset (&act, '\0', sizeof(act));

  // SA_SIGINFO pour utiliser sa_sigaction et
  // SA_RESTART pour relancer les fonctions bloquantes
  act.sa_flags = SA_SIGINFO|SA_RESTART;
  act.sa_sigaction = handleEnd;

  if (sigaction(SIGCHLD, &act, &oldAction) < 0) {
		die("Sigaction");
	}
}
