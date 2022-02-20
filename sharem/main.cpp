#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

#define ARRAY_SIZE 40000
#define MALLOC_SIZE 100000
#define SHM_SIZE 100000
#define SHM_MODE 0666

char array[ARRAY_SIZE];
int i = 1;

int
main(void) {
    int shmid;
    char *ptr, *shmptr;

    printf("global initialized int: %p\n", &i);
    printf("global array from: %p \t to: %p\n", (void*)&array[0], (void*)&array[ARRAY_SIZE]);
    printf("local int around: %p\n", (void*)&shmid);

    if ((ptr=(char*)malloc(MALLOC_SIZE)) == NULL){
        perror("malloc error");
        exit(1);
    }
    printf("malloc space from: %p \t to: %p\n", (void*)ptr, (void*)(ptr+MALLOC_SIZE));

    if ((shmid= shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget() failed");
        exit(1);
    }
    if ((shmptr=(char*)shmat(shmid, 0, 0)) == (char*)-1) {
        perror("shmat() failed");
        exit(1);
    }
    printf("shm atached at: %p \t to: %p\n", (void*)shmptr, (void*)(shmptr+SHM_SIZE));

    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shm del failed");
        exit(1);
    }
    return 0;
}