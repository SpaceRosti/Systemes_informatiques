#ifndef SHELL_H
#define SHELL_H

int buildArgv(char **argv,char commande[]);
void freeTab(char **argv,int argc);

#endif /* SHELL_H */
