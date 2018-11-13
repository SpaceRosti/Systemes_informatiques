#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"

#define MAXLECTURE 4096

void prepare_address( struct sockaddr_in *address, int port ) {
  // prépare l'adresse pour qu'elle soit utilisable par "bind"
  size_t addrSize = sizeof( address );
  memset(address, 0, addrSize);// remplis de 0 les places non utilisées
  address->sin_family = AF_INET;
  address->sin_addr.s_addr = htonl(INADDR_ANY); // écoute sur toute les adresses locales
  address->sin_port = htons(port); // converti le port en big-endian
}

int makeSocket(int port) {
  // crée le socket
  struct sockaddr_in address;
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  if( sock < 0 ) {
    die("Failed to create socket");
  }
  prepare_address( &address, port );
  if(bind(sock,(struct sockaddr *) &address,sizeof(address)) < 0){
    // lie le socket à une adresse
    die("Failed to bind socket");
  }
  if(listen(sock,5) < 0){
    // le serveur écoute les requêtes provenant de cette adresse
    die("Failled to listen to requests");
  }
  return sock;
}

void remplisBuffer(unsigned char *buffer){
  // remplis le buffer de nombre aléatoire
  int file;
  file = open("/dev/urandom", O_RDONLY);
  if(file<0){
    die("Impossible d'ouvrir le fichier");
  }
  if(read(file,buffer,MAXLECTURE) < 0){
    die("Impossible de remplir le buffer");
  }
  if(close(file)<0){
    die("Echec de la fermeture du fichier");
  }
}

void handleClient(int clientSock){
  int n; // Nombre de bits voulu
  int s = 0; // Nombre de bits déjà envoyé
  int nRead; // descripteur de fichier
  unsigned char buffer[MAXLECTURE];

  printf("Client connected\n");
  nRead = read(clientSock,&n,MAXLECTURE);
  if (nRead < 0){
    die("Echec de la lecture de la requête");
  }

  remplisBuffer(buffer);

  while(n>0){
    if (n <= MAXLECTURE - s){ // buffer contient suiffsament de nombre aléatoire
      if(write(clientSock,buffer,n)<0){
        die("Echec lors de l'écriture de la requête");
      }
      s = s + n;
      n = 0;
    }
    else if(n > MAXLECTURE - s){ // le buffer doit être vidé est re-remplis
      if(write(clientSock,buffer,MAXLECTURE)<0){
        die("Echec lors de l'écriture de la requête");
      }
      n = n - (MAXLECTURE-s);
      remplisBuffer(buffer);
      s=0;
    }
  }
}

void run(int sock){
  // lance ler serveur et gère le client
  while(1){
    struct sockaddr_in clientAddress;
    unsigned int clientLength = sizeof(clientAddress);
    int clientSock;
    clientSock = accept(sock,(struct sockaddr *) &clientAddress, &clientLength);
    if( clientSock < 0){
      die("Failed to accept client connection");
    }
    handleClient(clientSock);
    close(clientSock);
  }
}

int main(int argc, char *argv[]) {
  int sock;    /* Socket */
  int port;    /* Port du service */

  /* Initialisation */


  if (argc != 2) {
    fprintf(stderr, "USAGE: %s <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  port = atoi(argv[1]);

  /* Connection */

  sock = makeSocket(port);

  run(sock);

  close(sock);
}
