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
#include <netdb.h>
#include <time.h>

#include "common.h"

#define MAXLECTURE 500
#define MAXBUFFER 10000
#define MAXHEADER 1000

// Usage : <le fichier à envoyer> <adresse ip de sa machine>
// <adresse de l'envoyeur> <adresse du destinataire>

void makeHeader(char *header,char *from,char *to){
  // Ajoute les en-tête de manière à ce que le message passe l'anti-spam.
  time_t rawtime;
  time (&rawtime);
  char data[300];
  // convertit la date au format RFC 2822
  strftime(data, 300,"Date: %a, %d %b %Y %H:%M:%S +0100 (CET)\r\n",localtime(&rawtime));

  char temp[MAXBUFFER];
  memset(temp,0,MAXBUFFER);
  memset(header,0,MAXHEADER);
  strcpy(header,data);
  sprintf(temp,"From: <%s>\r\n",from);
  strcat(header,temp);
  memset(temp,0,MAXBUFFER);
  sprintf(temp,"To: <%s>\r\n",to);
  strcat(header,temp);
  strcat(header,"Subject: Salut!\r\n");
  strcat(header,"\r\n");
}

void readAndTestCode(char *answer){
  // Test si le code que renvoie le serveur est compris entre 200 et 355.
  // Si c'est le cas, continue normalement le programme, sinon le termine.
  char codeCara[4];
  int code;
  strncpy(codeCara,answer,3);
  code = atoi(codeCara);
  if (code > 200 && code < 355){
    printf("%s",answer);
    return;
  }
  else{
    printf("Wrong answer code from server: %s",answer);
    exit(EXIT_FAILURE);
  }
}

void lectureFichier(const char *nom,char *buffer){
  // Lis le fichier qui contient le message
  int file;
  file = open(nom, O_RDONLY);
  if(file<0){
    die("Impossible d'ouvrir le fichier");
  }
  if(read(file,buffer,MAXBUFFER) < 0){
    die("Impossible de remplir le buffer");
  }
  if(close(file)<0){
    die("Echec de la fermeture du fichier");
  }
}

/* Prépare l'adresse du serveur */
void prepare_address( struct sockaddr_in *address, const char *host, int port ) {
  size_t addrSize = sizeof( address );
  memset(address, 0, addrSize);
  address->sin_family = AF_INET;
  inet_pton( AF_INET, (char*) host, &(address->sin_addr) );
  address->sin_port = htons(port);
}

/* Construit le socket client */
int makeSocket( const char *host, int port ) {
  struct sockaddr_in address;
  int sock = socket(PF_INET, SOCK_STREAM, 0);
  if( sock < 0 ) {
    die("Failed to create socket");
  }
  prepare_address( &address, host, port );
  if( connect(sock, (struct sockaddr *) &address, sizeof(address)) < 0) {
    die("Failed to connect with server");
  }
  return sock;
}

int main(int argc, char *argv[]) {
  int sock;    /* Socket */
  char *host;  /* Adresse IP du serveur */
  int port;    /* Port du service */
  char data[100]; /* Buffer de reception */
  char commande[MAXBUFFER]; /* commande à envoyer */
  char buffer[MAXBUFFER]; /* Buffer du message */
  char header[MAXBUFFER]; /* Header */
  ssize_t n=0;

  if (argc != 5) {
    // Usage : <le fichier à envoyer> <l'adresse ip de sa machine>
    // <adresse de l'envoyeur> <adresse du destinataire>
    fprintf(stderr, "USAGE: %s <fichierTexte> <yourIP> <from> <to>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct hostent *hp = gethostbyname("smtp.unige.ch"); // résout l'adresse DNS
  if (hp == NULL) {
    printf("gethostbyname() failed\n");
  }

  host = inet_ntoa(*( struct in_addr*)( hp -> h_addr_list[0]));
  port = 25;

  // pépare la 1ère commande
  strcpy(commande,"HELO ");
  strcat(commande,"[");
  strcat(commande,argv[2]);
  strcat(commande,"]");
  strcat(commande,"\r\n");

  /* Connection */
  sock = makeSocket( host, port );

  // Lis le message de bienvenu
  n = read(sock,data,MAXLECTURE);
  if( n  < 0 ) {
    die( "Cannot receive data" );
  }
  readAndTestCode(data);
  memset(data,0,100);

  //--------------------------------------------------------------------
  // Envoie la commande "HELO MYIP"
  if( write(sock,commande,strlen(commande)) < strlen(commande) ) {
    die( "Cannot send the filename to retrieve" );
  }

  n = read( sock, data, MAXLECTURE );
  if( n  < 0 ) {
    die( "Cannot receive data" );
  }
  readAndTestCode(data);
  memset(data,0,100);

  //--------------------------------------------------------------------------
  // écrit et envoie la 2ème commande
  strcpy(commande,"MAIL FROM: <");
  strcat(commande,argv[3]);
  strcat(commande,">\r\n");

  if( write(sock,commande,strlen(commande)) < strlen(commande) ) {
    die( "Cannot send the filename to retrieve" );
  }

  n = read( sock, data, MAXLECTURE );
  if( n  < 0 ) {
    die( "Cannot receive data" );
  }
  readAndTestCode(data);
  memset(data,0,100);

  //------------------------------------------------------------------------
  // écrit et envoie la 3ème commande
  strcpy(commande,"RCPT TO: <");
  strcat(commande,argv[4]);
  strcat(commande,">\r\n");

  if( write(sock,commande,strlen(commande)) < strlen(commande) ) {
    die( "Cannot send the filename to retrieve" );
  }

  n = read( sock, data, MAXLECTURE );
  if( n  < 0 ) {
    die( "Cannot receive data" );
  }
  readAndTestCode(data);
  memset(data,0,100);

  //---------------------------------------------------------------------
  // écrit et envoie la commande qui demande au serveur d'attendre un message
  strcpy(commande,"DATA\r\n");
  if( write(sock,commande,strlen(commande)) < strlen(commande) ) {
    die( "Cannot send the filename to retrieve" );
  }

  n = read( sock, data, MAXLECTURE );
  if( n  < 0 ) {
    die( "Cannot receive data" );
  }
  readAndTestCode(data);
  memset(data,0,100);
  //------------------------------------------------------------------
  // Envoie le message au serveur
  makeHeader(header,argv[3],argv[4]); // pépare le header
  strcpy(commande,header); // ajoute le header à la commande
  lectureFichier(argv[1],buffer); // Place le fichier à envoyer dans le buffer
  strcat(commande,buffer); // ajoute le buffer à la commande
  strcat(commande,"\r\n.\r\n"); // ajoute le symbole de terminaison au message
  if( write(sock,commande,strlen(commande)) < strlen(commande) ) {
    die( "Cannot send the filename to retrieve" );
  }

  n = read( sock, data, MAXLECTURE );
  if( n  < 0 ) {
    die( "Cannot receive data" );
  }
  readAndTestCode(data);
  memset(data,0,100);
  //----------------------------------------------------
  // Indique au serveur que le client à terminé
  strcpy(commande,"QUIT\r\n");
  if( write(sock,commande,strlen(commande)) < strlen(commande) ) {
    die( "Cannot send the filename to retrieve" );
  }

  n = read( sock, data, MAXLECTURE );
  if( n  < 0 ) {
    die( "Cannot receive data" );
  }
  readAndTestCode(data);

  close(sock);

  exit(EXIT_SUCCESS);
}
