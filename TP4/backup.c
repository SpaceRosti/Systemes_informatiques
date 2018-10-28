#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include "backup.h"

void copieMultiple(int argc,char *argv[],int argA){ // lis les différents paramètres et choisis le comportmeen à adopter en fonction
  int cas = 0;

  struct stat fileInfo;
  if (stat(argv[argc-1],&fileInfo)==-1){
    cas = 1;
  }

  if(cas == 0 && S_ISDIR(fileInfo.st_mode)){ // le dernier paramètre est un dossier de destination qui existe
    for(int i=1;i<argc-1;i++){ // lis les paramètres qui précède le dernier
      struct stat Info;
      if (stat(argv[i],&Info)==-1){
        perror("stat");
        exit(0);
      }
      if(S_ISREG(Info.st_mode) || S_ISLNK(Info.st_mode)){ // le paramètre est un fichier ou un lien
        copieFichier(argv[i],argv[argc-1],argA); // copie dans le fichier destination
      }
      else if(S_ISDIR(Info.st_mode)){ // le paramètre est un répertoire
        char temp[100]="";
        strcpy(temp,argv[argc-1]);
        strcat(strcat(temp,"/"),argv[i]);
        if(modifCheck(argv[i],temp)==0){ // vérifie si le répertoire de destination existe déjà et s'il est différent(taille et date de modification)
          mkdir(temp,Info.st_mode);
          gereRepertoire(argv[i],argv[argc-1],argA); // gère les sous-répertoire
        }
        else if(argA == 1){ // option -a, modifie les droits du dossier sans modifier le dossier
          chmod(temp,Info.st_mode);
          gereRepertoire(argv[i],argv[argc-1],argA); // gère les sous-répertoire pour modifier leurs droits
        }
      }
    }

  }
  else if(cas == 0 && (S_ISREG(fileInfo.st_mode) || S_ISLNK(fileInfo.st_mode))){ // le dernier paramètre est un fichier/lien et il existe
    if (argc != 3){
      printf("Erreur trop de fichier passés en paramètre sans répertoire d'arrivé\n");
      exit(0);
    }
    else{
      copieDeuxFichier(argv[1],argv[2],cas,argA); // gère la copie pour deux fichiers
    }
  }
  else if(cas == 1){ // le dernier paramètre est un fichier/lien et il n'existe pas
    if (argc != 3){
      printf("Erreur trop de fichier passés en paramètre sans répertoire d'arrivé\n");
      exit(0);
    }
    else{
      copieDeuxFichier(argv[1],argv[2],cas,argA); // gère la copie pour deux fichiers
    }
  }
  else{
    printf("Mauvais renseignement des paramètres\n");
  }

}

void copieFichier(char *source,char *destination,int argA){ // copie un fichier/lien vers une destination
  FILE *fSource;
  FILE *fDest;
  char buffer[512];
  char temp[512];
  int NbLus;
  char retSource[512];

  strcpy(temp,destination);
  strcpy(retSource,source);

  if((fSource = fopen(source,"r"))==NULL){
    perror("fopen");
    exit(0);
  }

  // supprime le chemin pour accéder au fichier pour n'obtenir que son nom
  char *token;
  char *clean;
  token = strtok(source,"/");
  while(token != NULL){
    clean = token;
    token = strtok(NULL,"/");
  }

  strcat(strcat(temp,"/"),clean);
  if(modifCheck(retSource,temp)==0){ // vérifie si le fichier doit être modifié
    if((fDest = fopen(temp,"w")) == NULL){
      perror("fopen");
      fclose(fSource);
      exit(0);
    }

    while ((NbLus = fread(buffer, 1, 512, fSource)) != 0){ // écrit dans le fichier de destination
      fwrite(buffer, 1, NbLus, fDest);
    }

    fclose(fDest);
    gestionPerm(source,temp); // change les permissions
  }
  else if(argA == 1){ // change les permissions si l'option -a est présente
    gestionPerm(source,temp);
  }
  fclose(fSource);
}

void copieDeuxFichier(char *source,char *destination,int cas,int argA ){ // lorsque uniquement deux fichiers sont passés en paramètre
// si le fichier de destination existe deja, ne doti pas être modifié, mais que l'option -a est activée
if(cas==0 && modifCheck(source,destination)==1 && argA == 1){
  gestionPerm(source,destination);
  return;
}
// le fichier de destination existe déjà et est similaire au fichier source
else if(cas==0 && modifCheck(source,destination)==1){
  return;
}
  FILE *fSource;
  FILE *fDest;
  char buffer[512];
  char temp[512];
  int NbLus;

  strcpy(temp,destination);

  if((fSource = fopen(source,"r"))==NULL){
    perror("fopen");
    exit(0);
  }

  if((fDest = fopen(destination,"w"))==NULL){
    perror("fopen");
    exit(0);
  }

  // écrit dans le fichier de destination
  while ((NbLus = fread(buffer, 1, 512, fSource)) != 0){
    fwrite(buffer, 1, NbLus, fDest);
  }
    fclose(fDest);
    fclose(fSource);
    gestionPerm(source,destination);  // change les permissions
}

void gereRepertoire(char *source,char *destination,int argA){ // lis les sous-répertoires
  struct dirent *de;
  DIR *dr = opendir(source);
  while ((de = readdir(dr)) != NULL){ // lis les fichiers/dossiers des sous-répertoires
    if (strcmp(de->d_name,"..") && strcmp(de->d_name,".")){
      if(de->d_type == DT_REG){ // est un fichier
        char temp[100];
        char temp2[100];
        strcpy(temp2,destination);
        strcpy(temp,source);
        // effectue la copie, strcat permet de créer le  chemin relatif jusqu'au fichier à traiter
        copieFichier(strcat(strcat(temp,"/"),de->d_name),strcat(strcat(temp2,"/"),source),argA);
      }
      if(de->d_type == DT_DIR){ // est un dossier
        char temp[100];
        char temp4[100];
        strcpy(temp4,source);
        strcpy(temp,destination);

        strcat(strcat(temp4,"/"),de->d_name);
        struct stat rep;
        if(stat(temp4,&rep)==-1){
          perror("stat");
          exit(0);
        }

        char temp2[100];
        char temp3[100];
        // construit les chemins relatifs jusqu'aux dossiers/fichiers
        strcpy(temp2,source);
        strcpy(temp3,destination);
        strcat(strcat(temp2,"/"),de->d_name);
        strcat(strcat(temp3,"/"),temp2);
        strcat(strcat(strcat(strcat(temp,"/"),source),"/"),de->d_name);

        if(modifCheck(temp2,temp3)==0){ //si le dossier doit être modifié
          mkdir(temp,rep.st_mode);
          gereRepertoire(temp2,destination,argA); // appel récursif pour gérer les sous(sous)-répertoires
        }
        else if(argA == 1){ // pas de modification mais otpion -a, les droits vont être changés
          chmod(temp,rep.st_mode);
          gereRepertoire(temp2,destination,argA); // pour gérer les droits des sous-répertoires
        }
      }
    }
  }
  closedir(dr);
}

int modifCheck(char *source, char *destination) { // vérifie si le fichier/dossier de destination existe et s'il n'est pas similaire
	struct stat fileStat;
	if(stat(source, &fileStat)==-1){
    perror("stat");
    exit(0);
  }

	struct stat fileStatSource; // exitence
	if(stat(destination, &fileStatSource)==-1){
    return 0;
  }

	if(fileStatSource.st_mtime < fileStat.st_mtime || fileStat.st_size != fileStatSource.st_size) {
    return 0; // source plus récente ou fichiers de taille différente, remplace
	}
  else {
		return 1; // source moins récente et même taille de fichier, ne remplace pas
	}

}

void gestionPerm(char *source,char *destination){ // modifie les permissions
  struct stat fileInfo;
  if (stat(source,&fileInfo)==-1){
    perror("stat");
    exit(0);
  }
  chmod(destination,fileInfo.st_mode);
}

int gestionArg(int *argc,char *argv[]){ // vérifie si l'option -a est présente dans les paramètres
  int flagA = 0;
  int option;
  char *copie[100];

  while((option = getopt(*argc,argv,"a:"))!=-1){
    switch(option){
      case 'a':
        flagA = 1;
        break;
    }
  }

  if(flagA == 1){
    for(int i=1;i<*argc;i++){ // effectue une copie du tableau argv
      copie[i] = argv[i];
    }
    for(int i=2;i<*argc;i++){ // supprime -a du tableau argv
      argv[i-1] = copie[i];
    }
    *argc = *argc - 1; // ajuste agrc au nouveau tableau argv
  }

  return flagA;

}
