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
#include <semaphore.h>
#include <limits.h>

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "usage: %s [SHM_NAME] [SIZE IN BYTES]\n", argv[0]);
        return EXIT_FAILURE;
    }
    int size = atoi(argv[2]), fd;
    if(size <= 0) {
        fprintf(stderr, "Size must be bigger than 0\n");
        return EXIT_FAILURE;
    }
    
    if ((fd = shm_open(argv[1], O_RDWR, 0)) < 0) {
        perror("Eroare la shm_open in fiu");
        return EXIT_FAILURE;
    }
    if (ftruncate(fd, size) < 0) {
        perror("Eraore la ftruncate in fiu");
        close(fd);
        return EXIT_FAILURE;
    }

    void *map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("Eroare la mmap in fiu");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "/dev/shm%s", argv[1]);

    int ok = 0;
    if(ok == 1) {
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "sort -n -t: -k3 %s -o %s >1", path, path);
        if((system(command)) < 0) {
            perror("Eroare la system");
            munmap(map, size);
            return EXIT_FAILURE;
        }
    }
    else {
        execlp("sort", "sort", "-n", "-t:", "-k3", path, "-o", path, NULL);
        
        perror("Eraore la execlp(sort)");
        munmap(map, size);
        return EXIT_FAILURE;
    }

     munmap(map, size);
    return 0;
}