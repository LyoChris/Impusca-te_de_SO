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
#include <math.h>
#include <libgen.h>
#include <semaphore.h>
#include <limits.h>

void worker2(int* mapano, int fifo_fd) {
    int num, cont = 0;

    while(read(fifo_fd, &num, sizeof(int)) > 0) {
        mapano[cont] = num * num;
        cont++;
    }

    mapano[4097] = cont;
    mapano[4098] = 256;

    close(fifo_fd);
}

void worker3(int* mapano) {
    int shm_fd;

    while(mapano[4098] != 256) {}
    int count = mapano[4097];

     while((shm_fd = shm_open("/shm3toS", O_RDWR, 0)) == -1) {
        if(errno == ENOENT) {}
        else {
            perror("Eroare la shm_open work1");
            exit(1);
        }
    }

    if(ftruncate(shm_fd,  4096 * sizeof(int)) == -1) {
        perror("Eroare la ftruncate in worker1");
        exit(3);
    }

    int* shm_map = mmap(NULL, 4096 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_map == MAP_FAILED) {
        perror("Eroare la mmap work1");
        exit(2);
    }

    int suma = 0;
    for(int i = 0; i < count - 1; i++) {
        suma += mapano[i];
        
    }
    
    shm_map[0] = suma;
    shm_map[1] = mapano[count -1];
    shm_map[4095] = 257;
}

int main(int argc, char* argv[]) {
    int* mapano = mmap(NULL, 4098 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pid_t pid;

    mapano[4098] = 0;

    if((pid = fork()) == -1) {
        perror("Eroare la creare fiu in worker2+3");
        exit(1);
    }

    if(pid == 0) {
        //worker 3
        worker3(mapano);
        return 0;
    }
    else {
        //worker 2 
        int fd_fifo;
        while((fd_fifo = open("./fifo", O_RDONLY)) == -1) {
            if(errno == ENOENT) { continue; }
            perror("Eroare la fifo");
            exit(5);
        }

        worker2(mapano, fd_fifo);
    }
}