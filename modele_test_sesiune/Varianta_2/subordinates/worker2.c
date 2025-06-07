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
#include <semaphore.h>
#include <limits.h>

typedef struct vrajeala {
    int a, b, c, r;
} edgingpremmium;

int main(int argc, char* argv[]) {
    int fd_shm, count = 4096;

    while((fd_shm = shm_open("/shm_w1_w2", O_RDWR, 0)) == -1) {
        if(errno == ENOENT) {
            usleep(1000);
        }
        else {
            perror("Eroare la shm in worker2");
            exit(1);
        }
    }

    edgingpremmium* map = mmap(NULL, (count + 1)*sizeof(edgingpremmium), PROT_WRITE | PROT_READ, MAP_SHARED, fd_shm, 0);
    if(map == MAP_FAILED) {
        perror("Eroare la map");
        exit(6);
    }


    for(int i = 1; i <= map[0].a; i++) {
        if(map[i].r == 1) {
            map[i].r = (map[i].a + map[i].b + map[i].c) / 2;
        }
        else {
            map[i].r = 0;
        }
    }


    map[0].b = 200;


    munmap(map, (count + 1)*sizeof(edgingpremmium));


    close(fd_shm);

    return 0;
}