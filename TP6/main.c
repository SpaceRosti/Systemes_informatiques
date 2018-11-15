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

void makeHeader(char *header){
  time_t current_time;
  char* c_time_string;

  memset(header,0,MAXHEADER);
  strcpy(header,"Data:");
}

void readAndTestCode(char *answer){
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
    exit(EXIT_FAILURE);// A METTRE OU PAS ???
  }
}

void lectureFichier(const char *nom,char *buffer){
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
  char commande[100]="";
  char buffer[MAXBUFFER];
  ssize_t n=0;

  /* Initialisation */


  if (argc != 5) {
    fprintf(stderr, "USAGE: %s <fichierTexte> <youIP> <from> <to>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  lectureFichier(argv[1],buffer);

  struct hostent *hp = gethostbyname("smtp.unige.ch");
  if (hp == NULL) {
    printf("gethostbyname() failed\n");
  }

  host = inet_ntoa(*( struct in_addr*)( hp -> h_addr_list[0]));
  port = 25;
  strcpy(commande,"HELO ");
  strcat(commande,argv[2]);
  strcat(commande,"\r\n");
  /* Connection */

  sock = makeSocket( host, port );


  n = read(sock,data,MAXLECTURE);
  if( n  < 0 ) {
    die( "Cannot receive data" );
  }
  readAndTestCode(data);
  memset(data,0,100);

  //--------------------------------------------------------------------
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

  //-----------------------------------------------------
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

  //----------------------------------------------------------
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
  //-----------------------------------------------------
  strcpy(commande,buffer);
  strcat(commande,"\r\n.\r\n");
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
