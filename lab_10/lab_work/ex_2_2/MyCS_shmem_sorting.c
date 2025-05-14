#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define SEM_NAME "/peco_mutex"

void creeare_fisier() {
    int fd, count;
    char filename[256];

    printf("Dă numele fișierului: ");
    scanf(" %255s", filename);

    if((fd = open(filename, O_RDWR | O_TRUNC | O_CREAT, 0600)) < 0) {
        perror("eroare la creeare");
        exit(7);
    }

    printf("Da numarul de numere din fisier:");
    scanf(" %d", &count);

    if (ftruncate(fd, sizeof(int) * count) < 0) {
        perror("Eroare la ftruncate");
        close(fd);
        exit(8);
    }

    int *mmap_in = mmap(NULL, sizeof(int) * count, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_in == MAP_FAILED) {
        perror("Eroare la mmap");
        close(fd);
        exit(9);
    }
    for(int i = 0;i < count; i++) {
        int nr;
        scanf(" %d", &nr);
        *(mmap_in + i) = nr;
    }

    msync(mmap_in, sizeof(int) * count, MS_SYNC);

    munmap(mmap_in, sizeof(int) * count);
    close(fd);
    exit(0);


}

void afisare_date(char* filename) {
    int fd;
    if((fd = open(filename, O_RDONLY)) < 0) {
        perror("Eroare la open");
        exit(5);
    }

    struct stat st;
    if(fstat(fd, &st) < 0) {
        perror("fstat");
        exit(3);
    }

    int count = st.st_size / sizeof(int);
    int *mmap_in = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (mmap_in == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(4);
    }

    for (int i = 0; i < count; ++i) {
        printf("%d ", mmap_in[i]);
    }
    printf("\n");

    munmap(mmap_in, st.st_size);
    close(fd);
    exit(0);
}

void bubble_sort_mmap(int *v, int n, sem_t *sem) {
    int modificare = 1;

    while(modificare) {
        modificare = 0;
        for(int i = 0; i < n - 1; ++i) {
            sem_wait(sem);

            if(v[i] > v[i + 1]) {
                int aux = v[i];
                v[i] = v[i + 1];
                v[i + 1] = aux;
                modificare = 1;
            }

            sem_post(sem);
            usleep(100);
        }
    }
}


int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <file name>\n", argv[0]);
        exit(1);
    }

    if(strcmp(argv[1], "-o") == 0) {
        afisare_date(argv[2]);
    }

    if(strcmp(argv[1], "-i") == 0) {
        creeare_fisier(argv[2]);
    }

    int fd;
    if((fd = open(argv[1], O_RDWR)) < 0) {
        perror("open");
        exit(2);
    }

    struct stat st;
    if(fstat(fd, &st) < 0) {
        perror("fstat");
        exit(3);
    }

    if(st.st_size % sizeof(int) != 0) {
        fprintf(stderr, "Date de intrare incorecte.\n");
        exit(4);
    }
    
    int count = st.st_size / sizeof(int);

    int *mmap_in = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_in == MAP_FAILED) {
        perror("mmap");
        exit(5);
    }

    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(6);
    }

    bubble_sort_mmap(mmap_in, count, sem);

    munmap(mmap_in, st.st_size);
    close(fd);
    sem_close(sem);

    fprintf(stderr, "[PID %d] Sortare finalizată pe fișierul %s\n", getpid(), argv[1]);
    return 0;
}