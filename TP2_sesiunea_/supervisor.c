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
typedef struct {volatile int a,b,c,P,S;} triangle;

void numbers_to_binary_worker1(char* file, int fd_pipe) {
    int fd;
    if((fd = open(file, O_RDONLY)) == -1) {
        perror("Eroare la deschidere fisier");
        exit(3);
    }

    int count = 0;
    char line[1024], ch;
    while(read(fd, &ch, sizeof(char)) == 1) {
        if(ch == '\n') {
            if(count > 0) {
                line[count] = '\0';
                count = 0;
                edges tri;
                sscanf(line, "%d %d %d", &tri.a, &tri.b, &tri.c);

                write(fd_pipe, &tri, sizeof(edges));
            }
        }
        else {
            line[count++] = ch;
        }
    }

    if(count > 0) {
        line[count] = '\0';
        edges tri;
        sscanf(line, "%d %d %d", &tri.a, &tri.b, &tri.c);

        write(fd_pipe, &tri, sizeof(edges));
    }

    close(fd);
    close(fd_pipe);
}

void triangles_from_worker3(int fd_fi) {
    int contor1 = 0, contor2 = 0;
    triangle tri;

    printf("aolo1\n");
    while(read(fd_fi, &tri, sizeof(triangle)) > 0) {
        if(tri.P != 0) {
            printf("Tripleta %d,%d,%d reprezintă lungimile laturilor unui triunghi ce are perimetrul %d și aria %d.\n", tri.a, tri.b, tri.c, tri.P, tri.S);
            contor1++;
        }
        else {
            contor2++;
            printf("Tripleta %d,%d,%d nu poate reprezenta lungimile laturilor unui triunghi.\n", tri.a, tri.b, tri.c);   
        }
    }

    printf("Numar de triplete valide:%d. \n Numar de triplete invalide: %d.\n", contor1, contor2);

    close(fd_fi);
    printf("aolo2\n");
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "NUmar incorect de argumente pentru %s\n", argv[0]);
        exit(1);
    }

    int fd_pipe[2];
    pid_t pid;
    if(pipe(fd_pipe) == -1) {
        perror("Eroare la pipe");
        exit(1);
    }

    if(mkfifo("./fifo", 0600) == -1) {
        if(errno == EEXIST) {}
        else {
            perror("Eroare la creare fifo");
            exit(2);
        }
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

        printf("SUP1\n");

        numbers_to_binary_worker1(argv[1], fd_pipe[1]);

        printf("SUP2\n");

        int fd_fifo;
        if((fd_fifo = open("./fifo", O_RDONLY)) == -1) {
            perror("Eroare la deschidere fifo read");
            exit(3);
        }

        printf("SUP3\n");

        triangles_from_worker3(fd_fifo);

        printf("SUP4\n");
    }
}
