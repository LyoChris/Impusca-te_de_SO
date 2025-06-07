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

typedef struct vreajeala {
    volatile int state; // 0 - analyzer scrie | 3 - analyzer a terminat de scris | 1 - service citeste | 2 - service a pus cele doua trigrame si int-ul
    char word[1024];
    char trig1[4], trig2[4];
    int num;
} trigger;

int readable(char* file) {
    if(access(file, W_OK) == -1) return 0;
    return 1;
}

void trigs_to_serv(trigger* map, char* file) {
    int fd;
    if((fd = open(file, O_RDONLY)) == -1) {
        perror("Eroare la open fisier");
        exit(6);
    }

    int count = 0;
    char ch, word[1024];
    while(read(fd, &ch, sizeof(char)) > 0) {
        if(strchr(" .,?!1234567890", ch) != NULL) {
            if(count > 0) {
                word[count] = '\0';

                while(map->state != 0) usleep(500);
                strcpy(map->word, word);
                map->state = 1;
                count = 0;
            }
        }
        else {
            word[count++] = ch;
        }
    }

    if(count > 0) {
        word[count] = '\0';

        while(map->state != 0) usleep(500);
        strcpy(map->word, word);
        map->state = 1;
    }

    map->state = 3;

    close(fd);
}

void data_from_serv(trigger* map) {
    while(map->state != 2) {usleep(1000);}

    printf("Prima trigrama este: %s, ultima trigrama este %s, in total au fost %d trigrame.\n", map->trig1, map->trig2, map->num);
    exit(0);
}

int main(int argc, char *argv[]) {
    int fd_shm;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        exit(1);
    }
    else {
        if(readable(argv[1]) == 0) {
            fprintf(stderr, "Nu pot citi din fisierul %s\n", argv[1]);
            exit(2);
        }
    }

    if((fd_shm = shm_open("/anal_serv", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) {
        perror("Eroare la shm_open");
        exit(3);
    }

    if(ftruncate(fd_shm, sizeof(trigger)) == -1) {
        perror("Eroare la ftrunc");
        exit(5);
    }

    trigger* map = mmap(NULL, sizeof(trigger), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if(map == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(4);
    }

    map->state = 0;

    trigs_to_serv(map, argv[1]);

    data_from_serv(map);

    munmap(map, sizeof(trigger));
    close(fd_shm);

    return 0;
}