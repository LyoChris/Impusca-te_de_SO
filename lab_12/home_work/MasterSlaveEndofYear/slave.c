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
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <semaphore.h>
#include <limits.h>

typedef struct vrajeala {
    int a, b, c;
} report;

int file_opener(char * filename) {
    int fd;
    if((fd = open(filename, O_RDWR)) == -1) {
        perror("Eroare la deschiderea fisierului");
        exit(1);
    }
    return fd;
}

void file_closer(int fd) {
     if(close(fd) == -1) {
        perror("Eroare la inchiderea fisierului");
        exit(2);
    }
}

report suma_type_shit(char* filename) {
    report sum;
    int fd = file_opener(filename);

    read(fd, &sum.a, sizeof(int));
    read(fd, &sum.b, sizeof(int));
    read(fd, &sum.c, sizeof(int));
    fprintf(stdout, "%s cu vals: %d, %d, %d\n", filename, sum.a, sum.b, sum.c);
    return sum;
    file_closer(fd);
}

report parcurgere_dir_luna(char* cale_cur) {
    report sum;
    DIR *dir = opendir(cale_cur);
    if (dir == NULL) {
        perror("Eroare parcurgere fisiere!");
        exit (1);
    }

    struct dirent* entry;

    while((entry =  readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        
        char cale_completa[PATH_MAX];
        sprintf(cale_completa,"%s/%s", cale_cur, entry->d_name);
        report temp = suma_type_shit(cale_completa);
        sum.a+=temp.a;
        sum.b+=temp.b;
        sum.c+=temp.c;
        fprintf(stdout, "Sumele sunt: %d, %d, %d\n", sum.a, sum.b, sum.c);
    }
    closedir(dir);
    return sum;
}

int main(int argc, char* argv[]) {
    report sum = parcurgere_dir_luna(argv[1]);
    int shm1 = shm_open(argv[2], O_RDWR, 0);
    if (ftruncate(shm1, 3 * sizeof(int)) < 0) {
        perror("Eraore la ftruncate in fiu");
        file_closer(shm1);
        return EXIT_FAILURE;
    }

    int *map = mmap(NULL, 3*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm1, 0);
    if (map == MAP_FAILED) {
        perror("Eroare la mmap in fiu");
        file_closer(shm1);
        return EXIT_FAILURE;
    }
    file_closer(shm1);

    map[0] = sum.a;
    map[1] = sum.b;
    map[2] = sum.c;

    msync(map, MS_SYNC, 3 * sizeof(int));
    munmap(map, 3 * sizeof(int));
}