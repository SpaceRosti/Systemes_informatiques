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
  int fd;
  int *SHMfd;

  if( (fd = shm_open("etagere", O_RDWR, S_IRUSR | S_IWUSR)) == -1){
    die("shm_open etagere");
  }
  SHMfd = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
  if(SHMfd == MAP_FAILED){
    die("mmap");
  }
  sem_t *semaphore = sem_open("/semTest", O_RDWR);
  if (semaphore == SEM_FAILED) {
    die("sem_open");
  }

  action(semaphore,SHMfd);

  printf("Le serveur a fini de servir les 1010 pizzas.\n");

  if(munmap(SHMfd, sizeof(int)) == -1){
    die("munmap");
  }

  close(fd);
  if (sem_unlink("/semTest") < 0){
    perror("sem_close");
  }
  shm_unlink("etagere");
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
