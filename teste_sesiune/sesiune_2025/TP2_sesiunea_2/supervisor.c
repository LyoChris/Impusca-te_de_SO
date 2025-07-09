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


void number_sworker1(char* file, int fd_pipe) {
    FILE* filefd = fopen(file, "r");
    if(filefd == NULL) {
        perror("Eroare la deschidere");
        exit(2);
    }

    int nr;
    while(fscanf(filefd, "%d", &nr) == 1) {
        write(fd_pipe, &nr, sizeof(int));
    }

    fclose(filefd);
    close(fd_pipe);
}

void numbers_worker3(int* shm_map) {
    while(shm_map[4095] != 257) {}

    int sum  = 0, sum_cifre = 0;
    sum = shm_map[0] + shm_map[1];
    char suma[25];
    sprintf(suma, "%d", sum);

    for(int i = 0; i < strlen(suma); i++) {
        sum_cifre += (suma[i] - '0');
    }

    printf("Suma cifrelor este: %d.\n", sum_cifre);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Numar incorect de argumente.\n");
        exit(1);
    }

    int fd_pipe[2], shm_fd;
    pid_t pid;
    if(pipe(fd_pipe) == -1) {
        perror("Eroare la pipe");
        exit(1);
    }

    while((shm_fd = shm_open("/shm3toS", O_RDWR | O_TRUNC | O_CREAT, 0600)) == -1) {
        if(errno == ENOENT) {}
        else {
            perror("Eroare la shm_open work1");
            exit(1);
        }
    }

    if(ftruncate(shm_fd,  4096 * sizeof(int)) == -1) {
        perror("Eroare la ftruncate in worker1");
        exit(3);
    }

    int* shm_map = mmap(NULL, 4096 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_map == MAP_FAILED) {
        perror("Eroare la mmap work1");
        exit(2);
    }

    if((pid = fork()) == -1) {
        perror("Eroare la fork");
        exit(3);
    }

    if(pid == 0) {
        close(fd_pipe[1]);
        dup2(fd_pipe[0], STDIN_FILENO);
        close(fd_pipe[0]);

        execlp("./worker1", "worker1", NULL);
        
        perror("Eroare la execlp");
        exit(4);
    }
    else {

        close(fd_pipe[0]);

        number_sworker1(argv[1], fd_pipe[1]);

        numbers_worker3(shm_map);


    }
}
