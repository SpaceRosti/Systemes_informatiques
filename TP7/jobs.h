#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>
#include <sys/wait.h>

void creeProcessusEnfant(char **argv,int argc);
int detectBackground(char **argv,int argc);
void creeProcessusEnfantBackground(char **argv,int argc);
void gestionSignal();
void handleEnd();

#endif /* JOBS_H */
