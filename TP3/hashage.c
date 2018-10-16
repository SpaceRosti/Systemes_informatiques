#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include "hashage.h"

#define TAILLE_MAX 1000

void hashChaine(char *chaineHash,char *hashName) // calcule le hash d'une chaîne de caractère et l'affiche
{
  EVP_MD_CTX *mdctx;
  const EVP_MD *md;

  unsigned char md_value[EVP_MAX_MD_SIZE];
  int md_len, i;

  md = EVP_get_digestbyname(hashName);

  if(!md) { // détermine si le digest demandé existe
         printf("Unknown message digest %s\n", hashName);
         exit(1);
  }

  // série de fonctions pour calculer le hash
  mdctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(mdctx, md, NULL);
  EVP_DigestUpdate(mdctx, chaineHash, strlen(chaineHash));
  EVP_DigestFinal_ex(mdctx, md_value, &md_len);
  EVP_MD_CTX_free(mdctx);

  printf("Digest is: "); // affiche le digest
  for (i = 0; i < md_len; i++)
         printf("%02x", md_value[i]);
  printf("\n");
}


void hashFichier(char *nomFichier,char *hashName) // calcule le hash d'un fichier texte
{
  FILE* fichier = NULL;
  char chaine[TAILLE_MAX] = "";
  char chaineTotal[10000] = "";

  fichier = fopen(nomFichier, "r");

  if (fichier != NULL)
  {
    while (fgets(chaine, TAILLE_MAX, fichier) != NULL) // On lit le fichier tant qu'on ne reçoit pas d'erreur
    {
      strcat(chaineTotal,chaine); // concatène les différentes lignes du fichier pour obtenir une seule chaîne de caractère
    }

    fclose(fichier);

    hashChaine(chaineTotal,hashName); // calcule le hash de la chaîne obtenue
  }
  else{
    printf("fichier absent\n");
  }
}
