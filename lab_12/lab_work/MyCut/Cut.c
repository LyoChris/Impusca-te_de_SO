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
    pid_t pid;
    int status;

    if(-1 == (pid=fork()) ) {
        perror("Eroare la fork");  exit(1);
    }

    if (pid == 0) {
        char *arg[] = {
            "cut",
            "--fields",   "1,3,4,6",
            "-d:", 
            "--output-delimiter=\" -- \"",
            "/etc/passwd",
            NULL
        };

        execvp("cut", arg);

        perror("Eroare la exec");
        exit(10);
    }

    wait(&status);

    if( WIFEXITED(status) ) {
        switch( WEXITSTATUS(status) ) {
            case 10:  printf("Comanda cut nu a putut fi executata... (apelul exec a esuat).\n");  break;
            case  0:  printf("Comanda cut a fost executata cu succes!\n");  break;
            default:  printf("Comanda cut a fost executata, dar a esuat, terminandu-se cu codul de terminare: %d.\n", WEXITSTATUS(status) );
        }
    }
    else {
        printf("Comanda cut a fost terminata fortat de catre semnalul: %d.\n", WTERMSIG(status) );
    }

    return 0;

}