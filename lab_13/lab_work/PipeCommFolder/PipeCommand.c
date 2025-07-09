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

int main(int arg, char* argv[]) {
    pid_t pid1, pid2, pid3;
    int pipe_Cmd1toCmd2[2], pipe_Cmd2toCmd3[2], pipe_Cmd3toCmd4[2];

    if(pipe(pipe_Cmd3toCmd4) == -1) {
        fprintf(stderr, "Nu am putut crea creea ultimul canal anonim\n");
        perror("Cauza erorii");
        exit(1);
    }

    if((pid1 = fork()) == -1) {
        perror("Eroare la crearea primului fiu");
        exit(2);
    }

    if(pid1 == 0) {
        if(pipe(pipe_Cmd2toCmd3) == -1) {
            fprintf(stderr, "Nu am putut crea creea penultimul canal anonim\n");
            perror("Cauza erorii");
            exit(1);
        }

        if((pid2 = fork()) == -1) {
            perror("Eroare la crearea al doilea fiu");
            exit(2);
        }

        if(pid2 == 0) {
            if(pipe(pipe_Cmd1toCmd2) == -1) {
                fprintf(stderr, "Nu am putut crea creea primul canal anonim\n");
                perror("Cauza erorii");
                exit(1);
            }

            if((pid3 = fork()) == -1) {
                perror("Eroare la crearea al al treilea fiu");
                exit(2);
            }

            if(pid3 == 0) {
                close(pipe_Cmd1toCmd2[0]);
                if (dup2(pipe_Cmd1toCmd2[1], STDOUT_FILENO) == -1) {
                    perror("Eroare la redirectarea out in pipe 1");
                    exit(3);
                }
                close(pipe_Cmd1toCmd2[1]);
                execlp("ps", "ps", "-eo", "user,comm,pid", "--no-headers", NULL);
                
                perror("Eroare la ps");
                exit(4);
            }
            else {
                close(pipe_Cmd1toCmd2[1]);
                if (dup2(pipe_Cmd1toCmd2[0], STDIN_FILENO) == -1) {
                    perror("Eroare la redirectarea in in pipe 1");
                    exit(3);
                }
                close(pipe_Cmd1toCmd2[0]);

                close(pipe_Cmd2toCmd3[0]);
                if (dup2(pipe_Cmd2toCmd3[1], STDOUT_FILENO) == -1) {
                    perror("Eroare la redirectarea out in pipe 2");
                    exit(3);
                }
                close(pipe_Cmd2toCmd3[1]);

                execlp("tr", "tr", "-s", " ", NULL);

                perror("Eroare la tr");
                exit(4);
            }
        }
        else {
            close(pipe_Cmd2toCmd3[1]);
            if (dup2(pipe_Cmd2toCmd3[0], STDIN_FILENO) == -1) {
                perror("Eroare la redirectarea in in pipe 2");
                exit(3);
            }
            close(pipe_Cmd2toCmd3[0]);

            close(pipe_Cmd3toCmd4[0]);
            if (dup2(pipe_Cmd3toCmd4[1], STDOUT_FILENO) == -1) {
                perror("Eroare la redirectarea out in pipe 3");
                exit(3);
            }
            close(pipe_Cmd3toCmd4[1]);

            execlp("cut", "cut", "-d", " ", "-f", "1,2,3", "--output-delimiter=:", NULL);

            perror("Eroare la cut");
            exit(4);
        }
    }
    else {
        close(pipe_Cmd3toCmd4[1]);
        if (dup2(pipe_Cmd3toCmd4[0], STDIN_FILENO) == -1) {
            perror("Eroare la redirectarea in in pipe 3");
            exit(3);
        }
        close(pipe_Cmd3toCmd4[0]);

        execlp("sort", "sort", "-k2", "-t:", NULL);

        perror("Eroare la cut");
        exit(4);
    }
    
    return 0;
}