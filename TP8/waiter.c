#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#include "common.h"
#include "waiter.h"

#define VITESSE_SERVEUSE 2

int main(){
  sersPizza();
}

void sersPizza(){
  int fd,fd2;
  int *SHMfd;
  sem_t *sema;

  if( (fd = shm_open("etagere", O_RDWR, S_IRUSR | S_IWUSR)) == -1){
    die("shm_open etagere");
  }
  SHMfd = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
  if(SHMfd == MAP_FAILED){
    die("mmap");
  }
  if( (fd2 = shm_open("semPartage", O_RDWR, S_IRUSR | S_IWUSR)) == -1){
    die("shm_open semPartage");
  }
  sema = mmap(NULL,sizeof(sem_t*),PROT_READ | PROT_WRITE,MAP_SHARED,fd2,0);
  if(sema == MAP_FAILED){
    die("mmap semPartage");
  }

  if(sem_init(sema,1,1)== -1){
    die("sem_init");
  }

  action(sema,SHMfd);

  printf("Le serveur a fini de servir les 1010 pizzas.\n");

  if(munmap(SHMfd, sizeof(int)) == -1){
    die("munmap");
  }
  if(munmap(sema, sizeof(sem_t*)) == -1){
    die("munmap sema");
  }

  close(fd2);
  close(fd);
  shm_unlink("etagere");
  shm_unlink("semPartage");
  printf("Le serveur a fermé l'étagère et la semaphore.\n");
}

void action(sem_t *sem,int *SHMfd){
  int compteurPizza = 0;
  while(compteurPizza < 10){
    sleep(VITESSE_SERVEUSE);
    sem_wait(sem);
    if(*SHMfd > 0){
      *SHMfd = *SHMfd - 1;
      printf("Pizza servie,nombre de pizza restante: %d\n",*SHMfd);
      compteurPizza++;
    }
    sem_post(sem);
  }
}
