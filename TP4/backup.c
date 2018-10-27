#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include "backup.h"

void copieMultiple(int argc,char *argv[],int argA){ // Dans le cas où le dernier truc est un repertoire
  int cas = 0;

  struct stat fileInfo;
  if (stat(argv[argc-1],&fileInfo)==-1){
    cas = 1;
  }

  if(cas == 0 && S_ISDIR(fileInfo.st_mode)){
    for(int i=1;i<argc-1;i++){
      struct stat Info;
      if (stat(argv[i],&Info)==-1){
        perror("stat");
        exit(0);
      }
      if(S_ISREG(Info.st_mode) || S_ISLNK(Info.st_mode)){
        copieFichier(argv[i],argv[argc-1],argA);
      }
      else if(S_ISDIR(Info.st_mode)){
        char temp[100]="";
        strcpy(temp,argv[argc-1]);
        strcat(strcat(temp,"/"),argv[i]);
        if(modifCheck(argv[i],temp)==0){
          mkdir(temp,Info.st_mode);
          gereRepertoire(argv[i],argv[argc-1],argA);
        }
        else if(argA == 1){
          chmod(temp,Info.st_mode);
          gereRepertoire(argv[i],argv[argc-1],argA);
        }
      }
    }

  }
  else if(cas == 0 && (S_ISREG(fileInfo.st_mode) || S_ISLNK(fileInfo.st_mode))){
    if (argc != 3){
      printf("Erreur trop de fichier passés en paramètre sans répertoire d'arrivé\n");
      exit(0);
    }
    else{
      copieDeuxFichier(argv[1],argv[2],cas,argA);
    }
  }
  else if(cas == 1){
    if (argc != 3){
      printf("Erreur trop de fichier passés en paramètre sans répertoire d'arrivé\n");
      exit(0);
    }
    else{
      copieDeuxFichier(argv[1],argv[2],cas,argA);
    }
  }
  else{
    printf("Mauvais renseignement des paramètres\n");
  }

}

void copieFichier(char *source,char *destination,int argA){
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

  char *token;
  char *clean;
  token = strtok(source,"/");
  while(token != NULL){
    clean = token;
    token = strtok(NULL,"/");
  }

  strcat(strcat(temp,"/"),clean);
  if(modifCheck(retSource,temp)==0){
    if((fDest = fopen(temp,"w")) == NULL){
      perror("fopen");
      fclose(fSource);
      exit(0);
    }

    while ((NbLus = fread(buffer, 1, 512, fSource)) != 0){
      fwrite(buffer, 1, NbLus, fDest);
    }

    fclose(fDest);
    gestionPerm(source,temp);
  }
  else if(argA == 1){
    gestionPerm(source,temp);
  }
  fclose(fSource);
}

void copieDeuxFichier(char *source,char *destination,int cas,int argA ){
if(cas==0 && modifCheck(source,destination)==1 && argA == 1){
  gestionPerm(source,destination);
  return;
}
else if(cas==0 && modifCheck(source,destination)==1){
  return;
}
  FILE *fSource;
  FILE *fDest;
  char buffer[512];
  char temp[512];
  int NbLus;
  int flagChemin = 0;

  strcpy(temp,destination);

  if((fSource = fopen(source,"r"))==NULL){
    perror("fopen");
    exit(0);
  }

  if((fDest = fopen(destination,"w"))==NULL){
    perror("fopen");
    exit(0);
  }

  while ((NbLus = fread(buffer, 1, 512, fSource)) != 0){
    fwrite(buffer, 1, NbLus, fDest);
  }
    fclose(fDest);
    fclose(fSource);
    gestionPerm(source,destination);
}

void gereRepertoire(char *source,char *destination,int argA){
  struct dirent *de;
  DIR *dr = opendir(source);
  while ((de = readdir(dr)) != NULL){
    if (strcmp(de->d_name,"..") && strcmp(de->d_name,".")){
      if(de->d_type == DT_REG){
        char temp[100];
        char temp2[100];
        strcpy(temp2,destination);
        strcpy(temp,source);
        copieFichier(strcat(strcat(temp,"/"),de->d_name),strcat(strcat(temp2,"/"),source),argA);
      }
      if(de->d_type == DT_DIR){
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
        strcpy(temp2,source);
        strcpy(temp3,destination);
        strcat(strcat(temp2,"/"),de->d_name);
        strcat(strcat(temp3,"/"),temp2);
        strcat(strcat(strcat(strcat(temp,"/"),source),"/"),de->d_name);
        if(modifCheck(temp2,temp3)==0){
          mkdir(temp,rep.st_mode);
          gereRepertoire(temp2,destination,argA);
        }
        else if(argA == 1){
          chmod(temp,rep.st_mode);
          gereRepertoire(temp2,destination,argA);
        }
      }
    }
  }
  closedir(dr);
}

int modifCheck(char *source, char *destination) {
	struct stat fileStat;
	if(stat(source, &fileStat)==-1){
    perror("stat");
    exit(0);
  }

	struct stat fileStatSource;
	if(stat(destination, &fileStatSource)==-1){
    return 0;
  }

	if(fileStatSource.st_mtime < fileStat.st_mtime || fileStat.st_size != fileStatSource.st_size) {
    return 0; // source plus récente ou fichiers taille différente -> on va remplacer
	}
  else {
		return 1; // source moins récente et même taille de fichier -> on va pas remplacer
	}

}

void gestionPerm(char *source,char *destination){
  struct stat fileInfo;
  if (stat(source,&fileInfo)==-1){
    perror("stat");
    exit(0);
  }
  chmod(destination,fileInfo.st_mode);
}

int gestionArg(int *argc,char *argv[]){
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
    for(int i=1;i<*argc;i++){
      copie[i] = argv[i];
    }
    for(int i=2;i<*argc;i++){
      argv[i-1] = copie[i];
    }
    *argc = *argc - 1;
  }

  return flagA;

}
