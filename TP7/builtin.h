#ifndef BUILTIN_H
#define BUILTIN_H

void determineCommande(char **argv,int argc);
void commandExit(char **argv,int argc);
void commandeCd(const char *path);

#endif /* BUILTIN_H */
