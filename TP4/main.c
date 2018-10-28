#include <stdio.h>
#include <stdlib.h>
#include "listeFichier.h"
#include "backup.h"

void main(int argc,char *argv[])
{
  if(argc == 2){ // si un seul paramètre est renseigné
    afficheInfoComplet(argv[1]); // liste les éléments d'un dossier/fichier
  }
  else{
    int argA;
    argA = gestionArg(&argc,argv); // vérifie si l'option -a est présente et supprime -a du tableau argv
    if(argc == 2){printf("Pas assez de paramètres\n");exit(0);}
    copieMultiple(argc,argv,argA); // effectue les copies vers la destination
  }

}
