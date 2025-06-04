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


int contor_impare = 0;


int numbers_from_supervisor(int* map) {
    int nr, i = 0;

    while(read(STDIN_FILENO, &nr, sizeof(int)) == sizeof(int)) {
        if(nr % 2 != 0) {
            nr = nr * 2;
            contor_impare++;
        }
        map[i++] = nr;
    }

    map[4094] = i;
    return i;
}

int numbers_from_worker2(int* map, int size, int c) {
    int sum = 0;

    while(map[c - 1] == 0) {}

    for(int i = 0; i < size; i++) {
        sum += map[i];
    }

    return sum;
}



int main(int argc, char* argv[]) {
    int fd_fifo, fd_shm;
    int c = 4096;

    if((fd_fifo = open("./fifo1", O_WRONLY)) == -1) {
        perror("Eroare la deschidere fifo");
        exit(1);
    }

    
    if((fd_shm = shm_open("/shm_worker1", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) {
        perror("Eroare la deschidere fifo");
        exit(1);
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

    memset(shm_map, 0, c* sizeof(int));


    int coun = numbers_from_supervisor(shm_map);

    int suma = numbers_from_worker2(shm_map, coun, c);

    if (write(fd_fifo, &suma, sizeof(int)) != sizeof(int)) {
        perror("Eroare la write fifo");
    }

    if (write(fd_fifo, &contor_impare, sizeof(int)) != sizeof(int)) {
        perror("Eroare la write fifo");
    }

    munmap(shm_map, c* sizeof(int));
    close(fd_shm);
    close(fd_fifo);

    return 0;
}