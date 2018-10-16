#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include "hashage.h"
#include "lectureArgument.h"

void lecture(int argc,char *argv[]) // Lis les options passé en argument et affiche le(s) hash(s)
{
  int flagFichier = 0;
  int flagHash = 0;
  int option;

  while((option = getopt(argc,argv,"ft:"))!= -1){ // vérifie quelles options sont présentes
    switch(option){
      case 'f':
        flagFichier = 1;
        break;
      case 't':
        flagHash = 1;
        break;
    }
  }

  if (flagFichier==1 && flagHash==1){ // Pour -f et -t
    for(int i=4;i<argc;i++){
      hashFichier(argv[i],argv[3]); // appelle hashFichier pour chaque fichier passés en paramètre
    }
  }
  else if(flagFichier == 1 && flagHash == 0){ // Pour -f
    for(int i=2;i<argc;i++){
      hashFichier(argv[i],"sha1"); // appelle hashFichier pour chaque fichier passés en paramètre, en utilisant le hash sha1
    }
  }
  else if(flagFichier == 0 && flagHash == 1){ // Pour -t
    char chaineTotal[1000] = "";
    for(int i=3;i<argc;i++){ // concatène les paramètres pour former une chaîne de caractère
      strcat(chaineTotal,argv[i]);
      strcat(chaineTotal," "); // ajoute un espace entre chaque chaîne de caractère
    }
    chaineTotal[strlen(chaineTotal)-1] = '\0'; // supprime le dernier espace
    hashChaine(chaineTotal,argv[2]); // calcule le hash de la chaîne de caractère
  }
  else{ // sans option
    char chaineTotal[1000] = "";
    for(int i=1;i<argc;i++){ // concatène les paramètres pour former une chaîne de caractère
      strcat(chaineTotal,argv[i]);
      strcat(chaineTotal," "); // ajoute un espace entre chaque chaîne de caractère
    }
    chaineTotal[strlen(chaineTotal)-1] = '\0'; // supprime le dernier espace
    hashChaine(chaineTotal,"sha1"); // calcule le hash de la chaîne de caractère en utilisant sha1
  }
}
