#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <dirent.h>

void chainePermisison(struct stat fileInfo,char* retour){ //construit une chaîne de carctère pour les permissions

  if(S_ISREG(fileInfo.st_mode)){
    strcat(retour,"-");
  }
  else if(S_ISDIR(fileInfo.st_mode)){
    strcat(retour,"d");
  }
  else if(S_ISLNK(fileInfo.st_mode)){
    strcat(retour,"l");
  }
  else{
    strcat(retour,"?");
  }

  strcat(retour,(fileInfo.st_mode & S_IRUSR) ? "r" : "-");
  strcat(retour,(fileInfo.st_mode & S_IWUSR) ? "w" : "-");
  strcat(retour,(fileInfo.st_mode & S_IXUSR) ? "x" : "-");
  strcat(retour,(fileInfo.st_mode & S_IRGRP) ? "r" : "-");
  strcat(retour,(fileInfo.st_mode & S_IWGRP) ? "w" : "-");
  strcat(retour,(fileInfo.st_mode & S_IXGRP) ? "x" : "-");
  strcat(retour,(fileInfo.st_mode & S_IROTH) ? "r" : "-");
  strcat(retour,(fileInfo.st_mode & S_IWOTH) ? "w" : "-");
  strcat(retour,(fileInfo.st_mode & S_IXOTH) ? "x" : "-");
  strcat(retour,"\0");
}

void afficherInfo(char *alire){ // affiche les informations sur un fichier
  struct stat fileInfo;
  char buffer[80] = "";
  char permi[15]= "";
  time_t temps;
  struct tm wtf; // demander ce que c'est

  if (stat(alire,&fileInfo)==-1){
    perror("stat");
    exit(0);
  } // lis les infos de alire et les met dans fileInfo

  chainePermisison(fileInfo,permi);
  temps = fileInfo.st_mtime;
  localtime_r(&temps,&wtf);
  strftime(buffer,80,"%c",&wtf);

  printf("%s\t%ld\t%s\t%s\n",permi,fileInfo.st_size,buffer,alire);
}

void afficheInfoComplet(char *alire){ // ne gère pas les lien et fait tout apparaitre dans n'importe quel ordre
  afficherInfo(alire);
  char *temp[20];
  struct stat fileInfo;
  stat(alire,&fileInfo);
  if(S_ISDIR(fileInfo.st_mode)){ // si c'est un repertoire
    struct dirent *de;
    DIR *dr = opendir(alire); // besoin de faire un test ou pas ?
    while ((de = readdir(dr)) != NULL)
      if (strcmp(de->d_name,"..") && strcmp(de->d_name,".")){
        strcpy(temp,alire);
        afficheInfoComplet(strcat(strcat(temp,"/"),de->d_name));
      }
    closedir(dr);
  }
}
