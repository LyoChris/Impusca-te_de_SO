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

int word_count(char* str) {
    int nr = 0;
    int in_word = 0;

    for ( ; *str; ++str) {
        if (!isspace((char)*str)) {
            if (!in_word) {
                ++nr;
                in_word = 1;
            }
        }
        else {
            in_word = 0;
        }
    }
    return nr;
}

int generator_arg_exec(char *str, char **arg) {
    char*p;
    int i = 0;
    p = strtok(str, " ");
    while(p) {
        arg[i++] = p;
        p = strtok(NULL, " ");
    }
    arg[i] = NULL;
    return i;
}


int main(int argc, char* argv[]) {
    pid_t pid;
    int status;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s [STRING WITH ARGUMENTS]\n", argv[0]);
        exit(1);
    }

    char *cmdline = strdup(argv[1]);
    int n = word_count(cmdline);
    char **args = malloc((n + 1) * sizeof(char *));

    generator_arg_exec(cmdline, args);

    if((pid = fork()) == -1) {
        perror("Eroare la fork");
        exit(2);
    }

    if(pid == 0) {
        execvp(args[0],args);

        perror("Eroare la execlp");
        exit(125);
    }

    wait(&status);

    if( WIFEXITED(status) ) {
        switch( WEXITSTATUS(status) ) {
            case 125:  printf("Comanda specificata nu a putut fi executata... (apelul exec a esuat).\n");  break;
            case  0:  printf("Comanda specificata a fost executata cu succes!\n");  break;
            default:  printf("Comanda specificata a fost executata, dar a esuat, terminandu-se cu codul de terminare: %d.\n", WEXITSTATUS(status) );
        }
    }
    else {
        printf("Comanda specificata a fost terminata fortat de catre semnalul: %d.\n", WTERMSIG(status) );
    }
}