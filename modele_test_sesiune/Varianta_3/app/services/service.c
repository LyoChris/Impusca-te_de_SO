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

void path_to_worker(char* argv, char* path) {
    char resolved[PATH_MAX];
    if(realpath(argv, resolved) == NULL) {
        perror("Eroare la realpath");
        exit(3);
    }
    dirname(resolved);
    sprintf(path, "%s/%s", resolved, "worker");
}

void read_trigs_anal(trigger* map, int fd_pipe) {
    char set[4];

    while (1) {
        while (map->state == 0) usleep(500);
        
        for (size_t i = 0; i + 3 <= strlen(map->word); ++i) {
            set[0] = map->word[i];
            set[1] = map->word[i+1];
            set[2] = map->word[i+2];
            set[3] = '\0';
            write(fd_pipe, set, sizeof(set));
        }

        if (map->state == 3) break;
        map->state = 0;
    }

    close(fd_pipe);
}

void read_trigs_slave(trigger* map, char* fifo) {
    char set1[4], set2[4], tran[4];
    int fd_fifo, c = 0;

    if((fd_fifo = open(fifo, O_RDONLY)) == -1) {
        perror("Eroare la deschidere fifo");
        exit(8);
    }

    while(read(fd_fifo, &tran, sizeof(tran)) == sizeof(tran)) {
        if(c == 0) {
            strcpy(set1, tran);
        }
        c++;
        strcpy(set2, tran);
    }

    strcpy(map->trig1, set1);
    strcpy(map->trig2, set2);
    map->num = c;
    map->state = 2;
    close(fd_fifo);
}

int main(int argc, char* argv[]) {
    int fd_pipe[2], fd_shm;
    pid_t pid;

    if(pipe(fd_pipe) == -1) {
        perror("Eroare la pipe");
        exit(1);
    }

    if((pid = fork()) == -1) {
        perror("Eroare la fork");
        exit(2);
    }
    
    if(pid == 0) {
        close(fd_pipe[1]);
        dup2(fd_pipe[0], STDIN_FILENO);
        close(fd_pipe[0]);
        char path[PATH_MAX];
        path_to_worker(argv[0], &path);

        execlp(path, "worker", NULL);

        perror("Eroare la execlp");
        exit(4);
    }
    else {
        close(fd_pipe[0]);

        if(mkfifo("./fifosw", 0600) == -1) {
            if(errno == EEXIST) {}
            else {
                perror("Eroare la mkfifo");
                exit(5);
            }
        }

        while((fd_shm = shm_open("/anal_serv", O_RDWR, 0)) == -1) {
        if(errno == ENOENT) {
            usleep(1000);
        }
        else {
            perror("Eroare la shm in s");
            exit(1);
        }
    }

        trigger* map = mmap(NULL, sizeof(trigger), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
        if(map == MAP_FAILED) {
            perror("Eoare la mapare");
            exit(7);
        }

        read_trigs_anal(map, fd_pipe[1]);

        read_trigs_slave(map, "./fifosw");

        close(fd_shm);
    
        return 0;
    }
}