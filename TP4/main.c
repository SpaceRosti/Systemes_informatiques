#include <stdio.h>
#include <stdlib.h>
#include "listeFichier.h"
#include "backup.h"

void main(int argc,char *argv[])
{
  if(argc == 2){
    afficheInfoComplet(argv[1]);
  }
  else{
    int argA;
    argA = gestionArg(&argc,argv);
    if(argc == 2){printf("Pas assez de paramètres\n");exit(0);}
    copieMultiple(argc,argv,argA);
  }

}
