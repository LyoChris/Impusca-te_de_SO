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
typedef struct vrajeala { int a, b, c; } edging;
typedef struct vrajeala2 { int a, b, c, P, A; } triangle;

void triplets_to_binary_sex(char* file) {
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
                edging yes;
                sscanf(line, "%d %d %d", &yes.a, &yes.b, &yes.c);

                write(STDOUT_FILENO, &yes, sizeof(edging));
            }
        }
        else {
            line[count++] = ch;
        }
    }

    if(count > 0) {
        line[count] = '\0';
        edging yes;
        sscanf(line, "%d %d %d", &yes.a, &yes.b, &yes.c);

        write(STDOUT_FILENO, &yes, sizeof(edging));
    }

    close(fd);
    close(STDOUT_FILENO);
}

void numering_triangles(int fd_fi) {
    int contor1 = 0, contor2 = 0;
    triangle no;

    while(read(fd_fi, &no, sizeof(triangle)) > 0) {
        if(no.P != 0) {
            printf("Tripleta %d,%d,%d reprezintă lungimile laturilor unui triunghi ce are perimetrul %d și aria %d.\n", no.a, no.b, no.c, no.P, no.A);
            contor1++;
        }
        else {
            contor2++;
            printf("Tripleta %d,%d,%d nu poate reprezenta lungimile laturilor unui triunghi.\n", no.a, no.b, no.c);   
        }
    }

    printf("Perechi bune:%d, perechi nebune: %d.\n", contor1, contor2);


    exit(0);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "USAGE: %s [INPUT FILE] [STD]\n", argv[0]);
        exit(1);
    }

    int saved = atoi(argv[2]);

    triplets_to_binary_sex(argv[1]);

    fflush(stdout);
    dup2(saved, STDOUT_FILENO);
    close(saved);
    

    int fd_fifo;
    while((fd_fifo = open(FIFO, O_RDONLY)) == -1) {
        if(errno == ENOENT) { continue; }
        perror("Eroare la deschidere fifo");
        exit(2);
    }

    numering_triangles(fd_fifo);
    
    close(fd_fifo);
    return 0;
}