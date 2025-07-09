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

int IsCorrect(char ch) {
    if(isdigit(ch) || islower(ch) || isupper(ch)) return 1;
    return 0;
}

void sex_to_worker1(char* file, elem* map) {
    int fd;
    if((fd = open(file, O_RDONLY)) == -1) {
        perror("Eroare la open");
        exit(7);
    }

    char ch;
    int c = 0;
    while(read(fd, &ch, sizeof(char)) > 0) {
        if(IsCorrect(ch) == 1) map->set[c++] = ch;
    }

    map->con = c;

    close(fd);

}

void sex_from_worker1(elem* map) {
    while(map->ceva == 0) usleep(400);
    int sum = map->sum_digit + map->ceva;

    munmap(map, sizeof(elem));

    printf("Suma este: %d\n", sum);
    if(sum % 2 == 0) exit(0);
    exit(1);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "USAGE: %s <input file>\n", argv[0]);
        exit(2);
    }


    int fd_shm;
    if((fd_shm = shm_open("/shm_sex", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) {
        perror("Eroare la creare shm");
        exit(3);
    }

    if(ftruncate(fd_shm, sizeof(elem)) == -1) {
        perror("Eorare la ftruncate");
        exit(4);
    }


    elem* map = mmap(NULL, sizeof(elem), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if(map == MAP_FAILED) {
        perror("Eroare la mmap in sup");
        exit(5);
    }
    close(fd_shm);
    map->con = 0;

    sex_to_worker1(argv[1], map);

    sex_from_worker1(map);
}