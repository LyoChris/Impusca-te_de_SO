#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#include <limits.h>

int main(int argc, char* argv[]) {
    int fd_shm, c = 4096;

    
    while((fd_shm = shm_open("/shm_worker1", O_RDWR, 0600)) == -1) {
        if(errno == ENOENT) {
            usleep(1000);
        }
        else {
            perror("Eroare la shm in worker2");
            exit(1);
        }
    }

    
    if (ftruncate(fd_shm, c* sizeof(int)) == -1) {
        perror("Eroare la ftruncate");
        exit(2);
    }

    int* shm_map = mmap(NULL, c* sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
     if (shm_map == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(3);
    }

    
    int count = shm_map[4094];

    for(int i = 0; i < count; i++) {
        shm_map[i] = shm_map[i] * shm_map[i];
    }

    shm_map[4095] = -1;

    munmap(shm_map, c* sizeof(int));
    close(fd_shm);

    return 0;


}