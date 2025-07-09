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

typedef struct {volatile int a,b,c;} edges;
typedef struct {volatile int a,b,c,r;} edgesr;
typedef struct {volatile int a,b,c,P,S;} triangle;

int CanBeTriangle(int a, int b, int c) {
    if (a <= 0 || b <= 0 || c <= 0) return 0;
    if (a + b <= c) return 0;
    if (a + c <= b) return 0;
    if (b + c <= a) return 0;
    return 1;
}


void edges_from_sup_to_work2(edgesr* map) {
    edges tri;

    while(read(STDIN_FILENO, &tri, sizeof(tri)) > 0) {
        map[map[0].a].a = tri.a;
        map[map[0].a].b = tri.b;
        map[map[0].a].c = tri.c;
        map[map[0].a++].r = CanBeTriangle(tri.a, tri.b, tri.c);
        map[0].b++;
    }

    map[0].b = 200; //flag pentru citire in worker2

    printf("%d %d\n", map[0].a, map[0].b);

}

int main(int argc, char* argv[]) {
    int shm_fd;

    printf("WORKER11\n");

    while((shm_fd = shm_open("/shm1to2", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) {
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

    printf("WORKER12\n");

    edgesr* map = mmap(NULL, 4097 * sizeof(edgesr), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(map == MAP_FAILED) {
        perror("Eroare la mmap work1");
        exit(2);
    }

    printf("WORKER13\n");

    map[0].a = 1; //il voi folosi drept contor pentru mapare;

    edges_from_sup_to_work2(map);

    printf("WORKER14\n");
}