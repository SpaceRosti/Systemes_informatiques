#ifndef SHELL_H
#define SHELL_H

int buildArgv(char **argv,char commande[]);
void freeTab(char **argv,int argc);
void gestionDesSignaux();
void sigintHandler(int sig_num);

#endif /* SHELL_H */
