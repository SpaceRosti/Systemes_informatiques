#ifndef LISTEFICHIER_H_INCLUDED
#define LISTEFICHIER_H_INCLUDED

#include <stdio.h>

void afficherInfo(char *alire);
char* chainePermisison(struct stat fileInfo);
void afficheInfoComplet(char *alire);

#endif
