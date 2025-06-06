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
#include <semaphore.h>
#include <limits.h>

#define FIFO "./fifo1"

void triplets_to_binary_sex(char* file) {
    int fd;
    if((fd = open(file, O_RDONLY)) == -1) {
        perror("Eroare la deschidere fisier");
        exit(3);
    }

    int count = 0;
    char line[1024], ch;
    while(read(fd, &ch, sizeof(char)) > 0) {
        if(ch == '\n') {
            if(count > 0) {
                line[count] = '\0';
                count = 0;
                int a, b, c;
                sscanf(line, "%d %d %d", &a, &b, &c);

                write(STDOUT_FILENO, &a, sizeof(int));
                write(STDOUT_FILENO, &b, sizeof(int));
                write(STDOUT_FILENO, &c, sizeof(int));
                //printf("%d %d %d\n", a, b, c);
            }
        }
        else {
            line[count++] = ch;
        }
    }

    if(count > 0) {
        line[count] = '\0';
        int a, b, c;
        sscanf(line, "%d %d %d", &a, &b, &c);

        write(STDOUT_FILENO, &a, sizeof(int));
        write(STDOUT_FILENO, &b, sizeof(int));
        write(STDOUT_FILENO, &c, sizeof(int));
        //printf("%d %d %d\n", a, b, c);
    }

    close(fd);
}

void triplets_to_binary(char* file, int fd_pipe) {
    int fd;
    if((fd = open(file, O_RDONLY)) == -1) {
        perror("Eroare la deschidere fisier");
        exit(3);
    }

    int count = 0;
    char line[1024], ch;
    while(read(fd, &ch, sizeof(char)) > 0) {
        if(ch == '\n') {
            if(count > 0) {
                line[count] = '\0';
                count = 0;
                int vec[3], i = 0;
                char *p = strtok(line, " ");
                while(p) {
                    vec[i++] = atoi(p);
                    p = strtok(NULL, " ");
                }

                printf("%d %d %d\n", vec[0], vec[1], vec[2]);
            }
        }
        else{
            line[count++] = ch;
        }
    }

    if(count > 0) {
            //procesare
            line[count] = '\0';
            int vec[3], i = 0;
            char *p = strtok(line, " ");
            while(p) {
            vec[i++] = atoi(p);
            p = strtok(NULL, " ");
        }

        printf("%d %d %d\n", vec[0], vec[1], vec[2]);
    }

    close(fd);

}

void numering_triangles(int fd_fi) {
    int a, b, c, P, A, contor1 = 0, contor2 = 0;

    while(read(fd_fi, &a, sizeof(int)) > 0) {
        read(fd_fi, &b, sizeof(int));
        read(fd_fi, &c, sizeof(int));
        read(fd_fi, &P, sizeof(int));
        read(fd_fi, &A, sizeof(int));

        if(P != 0) {
            printf("Tripleta %d,%d,%d reprezintă lungimile laturilor unui triunghi ce are perimetrul %d și aria %d.\n", a, b, c, P, A);
            contor1++;
        }
        else {
            contor2++;
            printf("Tripleta %d,%d,_%d nu poate reprezenta lungimile laturilor unui triunghi.\n", a, b, c);   
        }
    }

    printf("Perechi bune:%d, perechi nebune: %d.\n", contor1, contor2);

}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "USAGE: %s [INPUT FILE]\n", argv[0]);
        exit(1);
    }

    int pipe_fd[2];
    if(pipe(pipe_fd) == -1) {
        perror("Eroare la pipe");
        exit(1);
    }

    int fd_fifo;
    while((fd_fifo = open(FIFO, O_RDONLY)) == -1) {
        if(errno == ENOENT) continue;
        perror("Eroare la deschidere fifo");
        exit(2);
    }

    //triplets_to_binary(argv[1], pipe_fd[1]);
    triplets_to_binary_sex(argv[1]);

    numering_triangles(fd_fifo);


}

