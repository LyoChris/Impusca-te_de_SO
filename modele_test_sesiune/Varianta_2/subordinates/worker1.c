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
#include <semaphore.h>
#include <limits.h>

typedef struct vrajeala { int a, b, c; } edging;
typedef struct vrajeala3 { int a, b, c, r; } edgingpremmium;
typedef struct vrajeala2 { int a, b, c, P, A; } triangle;

int IsTriangle(int a, int b, int c) {
    if (a <= 0 || b <= 0 || c <= 0) {
        return 0;
    }
    if (a + b <= c) return 0;
    if (a + c <= b) return 0;
    if (b + c <= a) return 0;
    return 1;
}

int Area(int a, int b, int c, int p) {
    return sqrt(p*(p-a)*(p-b)*(p-c));
}

edgingpremmium* map;

void numbers_to_worker2(int fd, int fd_pipe) {
    edging yes;
    while(read(fd_pipe, &yes, sizeof(edging)) == sizeof(edging)) {
        map[map[0].a].a = yes.a;
        map[map[0].a].b = yes.b;    
        map[map[0].a].c = yes.c;
        map[map[0].a++].r = IsTriangle(yes.a, yes.b, yes.c);
    }

    close(fd_pipe);

}

void numbers_from_worker2(int fd_fifo) {

    while(map[0].b != 200) {}

    for(int i = 1; i < map[0].a; i++) {
        triangle no;
        no.a = map[i].a;
        no.b = map[i].b;
        no.c = map[i].c;
        no.P = map[i].r * 2;
        no.A = Area(map[i].a, map[i].b ,map[i].c, map[i].r);
        write(fd_fifo, &no, sizeof(triangle));
    }

    close(fd_fifo);
}

int main(int argc, char* argv[]) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    int pipe_fd[2];
    pid_t pid;

    if(pipe(pipe_fd) == -1) {
        perror("Eroare la pipe");
        exit(1);
    }

    if(mkfifo("./fifo1", 0600) == -1) {
        if(errno == EEXIST) {}
        else {
            perror("Eroare la fifo1");
            exit(4);
        }
    }
    
    if((pid = fork()) == -1) {
        perror("Eroare la fork");
        exit(2);
    }

    if(pid == 0) {
        close(pipe_fd[0]);
        int stout = dup(STDOUT_FILENO);
        char s[16];
        sprintf(s, "%d", stout);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        char path[4096];
        sprintf(path, "%s/%s", cwd, "coordonator/supervisor");

        execlp(path, "supervisor", argv[1], s, NULL);

        perror("Eroare la exec");
        exit(3);
    }
    else {
        close(pipe_fd[1]);
         int fd_fifo;
    
        int fd;
        if((fd = shm_open("/shm_w1_w2", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) {
            perror("Eroare la shm");
            exit(6);
        }

        int count = 4096;
        if (ftruncate(fd, (count + 1)*sizeof(edgingpremmium)) == -1) {
            perror("Eroare la ftruncate");
            exit(2);
        }
        
        map = mmap(NULL, (count + 1)*sizeof(edgingpremmium), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
        if(map == MAP_FAILED) {
            perror("Eroare la map");
            exit(6);
        }
        map[0].a = 1;

        numbers_to_worker2(fd, pipe_fd[0]);

        if((fd_fifo = open("./fifo1", O_WRONLY)) == -1) {
            perror("Eroare la open fifo");
            exit(7);
        }

        numbers_from_worker2(fd_fifo);

        munmap(map,(count+1)*sizeof(edgingpremmium));
        close(fd);
    }

}