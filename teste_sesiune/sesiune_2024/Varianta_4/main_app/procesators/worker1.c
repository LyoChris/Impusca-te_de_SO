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

typedef struct vrakjeala {
    char set[4096];
    volatile int con;
    int sum_digit;
    int ceva;
} elem;

int sum_digits;

void sex_from_sup(elem* map, int fd_fifo) {
    sum_digits = 0;
    while(map->con == 0) usleep(300);
    for(int i = 0; i < map->con; i++) {
        if(isdigit(map->set[i])) {
            sum_digits += (map->set[i] - '0');
        }
        else {
            char ch = map->set[i];
            write(fd_fifo, &ch, sizeof(char));
        }
    }

    close(fd_fifo);
}

void sex_from_work2(elem* map) {
    int total_sum = 0, nr;
    unsigned char ch;

    while((read(STDIN_FILENO, &ch, sizeof(char)) > 0) && (ch != '$')) {
        total_sum += (int)ch;
    }

    read(STDIN_FILENO, &nr, sizeof(int));
    total_sum -= nr;

    map->ceva = total_sum;
    map->sum_digit = sum_digits;
}

int main(int argc, char* argv[]) {
    int fd_fifo, fd_shm;
    
    while((fd_shm = shm_open("/shm_sex", O_RDWR, 0)) == -1) {
        if(errno == ENOENT) continue;
        perror("Eroare la shm_open work1");
        exit(3);
    }

    elem* map = mmap(NULL, sizeof(elem), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if(map == MAP_FAILED) {
        perror("Eroare la mmap in sup");
        exit(5);
    }
    close(fd_shm);

    if((fd_fifo = open("./fifosw", O_WRONLY)) == -1) {
        perror("Eroare la open fifo W");
        exit(6);
    }

    sex_from_sup(map, fd_fifo);

    sex_from_work2(map);

    return 0;
}