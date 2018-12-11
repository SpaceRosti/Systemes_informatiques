#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#include "cook.h"
#include "common.h"

sem_t sem1;

#define ATTENTE_MAX 2

int main(){
  sharedMemory();
}

void pizza(){
  srand(time(NULL));
  float tps = rand();
  tps = (tps/RAND_MAX)*ATTENTE_MAX+1;
  printf("pépare la pizza pendant %f secondes\n",tps);
  sleep(tps);
  printf("pizza prete\n");
}

void sharedMemory(){
  int fd;
  int fd2;
  int *SHMfd;
  sem_t *sema;

  if( (fd = shm_open("etagere", O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1){
    die("shm_open etagere");
  }
  if( ftruncate(fd, sizeof(int)) == -1){
    die("ftruncate");
  }
  SHMfd = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
  if(SHMfd == MAP_FAILED){
    die("mmap etagere");
  }

  if( (fd2 = shm_open("semPartage", O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1){
    die("shm_open semPartage");
  }
  if( ftruncate(fd2, sizeof(sem_t*)) == -1){
    die("ftruncate");
  }
  sema = mmap(NULL,sizeof(sem_t*),PROT_READ | PROT_WRITE,MAP_SHARED,fd2,0);
  if(sema == MAP_FAILED){
    die("mmap semPartage");
  }

  if(sem_init(&sem1,1,1)== -1){
    die("sem_init");
  }

  action(&sem1,SHMfd);

  if(munmap(SHMfd, sizeof(int)) == -1){
    die("munmap SHMfd");
  }
  if(munmap(sema, sizeof(sem_t*)) == -1){
    die("munmap sema");
  }
  close(fd);
  close(fd2);
}

void action(sem_t *sem,int *SHMfd){
  int dodo = 0;
  int compteurPizza = 0;
  while(compteurPizza < 10){
    if(dodo == 0){
      pizza();
      compteurPizza++;
      sem_wait(sem);
      *SHMfd = *SHMfd + 1;
      printf("Pizza mise dans l'étagère, nombre de pizzas: %d\n",*SHMfd);
      if(*SHMfd == 3){
        dodo = 1;
        printf("Le cuisinier se repose\n");
      }
      sem_post(sem);
    }
    else{
      sleep(1);
      sem_wait(sem);
      if(*SHMfd <3){
        dodo = 0;
      }
      sem_post(sem);
    }
  }
}
