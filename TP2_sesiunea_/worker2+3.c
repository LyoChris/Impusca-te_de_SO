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

typedef struct {volatile int a,b,c,r;} edgesr;
typedef struct {volatile int a,b,c,P,S;} triangle;

int Area(int a, int b, int c, int p) {
    int Area = sqrt(p*(p-a)*(p-b)*(p-c));
    return Area;
}

void worker2(edgesr* mapano) {

    printf("WORKER21\n");

    int shm_fd;
    while((shm_fd = shm_open("/shm1to2", O_RDWR, 0)) == -1) {
        if(errno == ENOENT) {}
        else {
            perror("Eroare la shm_open work1");
            exit(1);
        }
    }

    if(ftruncate(shm_fd,  4097 * sizeof(edgesr)) == -1) {
        perror("Eroare la ftruncate in worker1");
        exit(3);
    }

    edgesr* map = mmap(NULL, 4097 * sizeof(edgesr), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(map == MAP_FAILED) {
        perror("Eroare la mmap work1");
        exit(2);
    }

    printf("WORKER22\n");


    printf("Numeere: %d, %d, %d, %d\n", map[1].a, map[1].b, map[1].c, map[1].r);

    //sleep (4);
    while(map[0].b != 200) {}

    for(int i = 1; i <= map[0].a; i++) {
        mapano[i].a = map[i].a;
        mapano[i].b = map[i].b;
        mapano[i].c = map[i].c;

        if(map[i].r == 1) {
            mapano[i].r = (map[i].a + map[i].b + map[i].c) / 2;
        }
        else {
            mapano[i].r = 0;
        }
    }

    mapano[0].b = 200;

    printf("WORKER24\n");
}

void worker3(edgesr* mapano, int fd_fifo) {

    printf("WORKER31\n");

    while(mapano[0].b != 200) {}

    printf("%d\n", mapano[0].a);

    printf("WORKER32\n");

    for(int i = 1; i < mapano[0].a; i++) {
        triangle tri;
        tri.a = mapano[i].a;
        tri.b = mapano[i].b;
        tri.c = mapano[i].c;
        tri.P = mapano[i].r * 2;
        tri.S = Area(mapano[i].a, mapano[i].b, mapano[i].c, mapano[i].r);

        printf("Numeere: %d, %d, %d, %d\n", tri.a, tri.b, tri.c, tri.P);

        write(fd_fifo, &tri, sizeof(triangle));
    }

    printf("WORKER33\n");

    close(fd_fifo);
}

int main(int argc, char* argv[]) {
    edgesr* mapano = mmap(NULL, 4097 * sizeof(edgesr), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pid_t pid;

    if((pid = fork()) == -1) {
        perror("Eroare la creare fiu in worker2+3");
        exit(1);
    }

    if(pid == 0) {
        //worker 2
        worker2(mapano);
        return 0;
    }
    else {
        //worker 3 
        int fd_fifo;
        while((fd_fifo = open("./fifo", O_WRONLY)) == -1) {
            if(errno == ENOENT) { continue; }
            perror("Eroare la fifo");
            exit(5);
        }

        worker3(mapano, fd_fifo);
    }
}