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
#include <errno.h>
#include <semaphore.h>
#include <limits.h>

int is_hex_char(char ch) {
    if(ch >= '0' && ch <= '9') return 1;
    if(ch >= 'A' && ch <= 'F') return 1;
    if(ch >= 'a' && ch <= 'f') return 1;
    return 0; 
}

int main(int argc, char* argv[]) {
    pid_t pid1, pid2;
    int pipe_P0toP1[2], pipe_P2toP0[2];

    if(pipe(pipe_P0toP1) == -1) {
        perror("Eroare la crearea primului pipe");
        exit(1);
    }

    if(pipe(pipe_P2toP0) == -1) {
        perror("Eroare la crearea al doilea pipe");
        exit(1);
    }

    if((pid1 = fork()) == -1) {
        perror("Eroare la fork");
        exit(2);
    }

    if(pid1 == 0) {
        char ch;
        close(pipe_P0toP1[1]);
        close(pipe_P2toP0[0]);
        close(pipe_P2toP0[1]);

        if(mkfifo("./fifo1", 0600) == -1) {
            if(errno == EEXIST) {

            }
            else {
                perror("Eroare la mkfifo");
                exit(3);
            }
        }

        int fd;
        if((fd = open("./fifo1", O_WRONLY)) == -1) {
            perror("Eroare la open");
            exit(3);
        }

        while(read(pipe_P0toP1[0], &ch, 1) > 0) {
            if(is_hex_char(ch) == 1) {
                if(ch >= 'A' && ch <= 'F') {
                    ch = tolower(ch); 
                }
                printf("%c", ch);

                write(fd, &ch, 1);
            }
        }
        printf("\n");
        close(pipe_P0toP1[0]);
        close(fd);
        return 0;
    }
    else {
        if((pid2 = fork()) == -1) {
            perror("Eroare la fork");
            exit(2);
        }

        if(pid2 == 0) {
            int frec[16] = {0}, count = 0;
            char ch;
            close(pipe_P0toP1[0]);
            close(pipe_P0toP1[1]);
            close(pipe_P2toP0[0]);

            int fd;
            if((fd = open("./fifo1", O_RDONLY)) == -1) {
            perror("Eroare la open");
            exit(3);
            }

            while(read(fd, &ch, 1) > 0) {
                if(ch >= 'a' && ch <= 'f') {
                    printf("%d ",ch - 'a' + 10);
                    frec[ch - 'a' + 10]++;
                }
                if(ch >= '0' && ch <= '9') {
                    frec[ch - '0']++;
                }
            }
            for(int i = 0; i < 16; i++) {
                printf("%d %d\n", i, frec[i]);
                if(frec[i] > 0){
                    printf("Intrare nr: %d\n", i);
                    count++;
                }
            }

            write(pipe_P2toP0[1], &count, sizeof(int));
            close(pipe_P2toP0[1]);
            close(fd);
            return 0;
        }

        int fd;
        if((fd = open("./input-data.txt", O_RDONLY)) == -1) {
            perror("Eroare la open");
            exit(3);
        }

        close(pipe_P0toP1[0]);
        close(pipe_P2toP0[1]);
        char ch;
        while((read(fd, &ch, 1)) > 0) {
            write(pipe_P0toP1[1], &ch, 1);
        }
        printf("\n");
        close(pipe_P0toP1[1]);

        int a;
        read(pipe_P2toP0[0], &a, sizeof(int));

        printf("%d\n", a);
    }
}