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

void exit_shell(int error) {
    switch (error) {
        case 1: printf("Eroare la citire, Shell se va inchide\n"); exit(1);
        case 2: exit(0);
        case 3: printf("Eroare la execvp, Shell se inchide\n"); exit(3);
        case 4: printf("Eroare la fork, Shell se inchide\n"); exit(4);
        default: error = 5; 
    }
}

void Shell() {
    char* line = NULL;
    size_t len = 0, nread;

    printf("MyShell> ");
    if((nread = getline(&line, &len, stdin)) == -1) {
        perror("Eroare la getline");
        free(line);
        exit_shell(1);
    }

    if (line[nread-1] == '\n') {
        line[nread-1] = '\0';
        --nread;
    }

    if(strcmp(line, "exit") == 0) exit_shell(2);
    //system(line);
    char* argvs[1000];
    int count = 0;
    char *token = strtok(line, " \t");
    while(token) {
        argvs[count++] = token;
        token = strtok(NULL, " \t");
    }
    argvs[count] = NULL;

    if(count == 0) {
        free(line);
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Eroare la fork");
        exit_shell(4);
    } 
    else { 
        if (pid == 0) {
            execvp(argvs[0], argvs);

            perror("Eroare la execvp");
            exit_shell(3);
        }
        else {
            int status;
            waitpid(pid, &status, 0);
            }
    }

    free(line);

}

int main(int argc, char* argv[]) {
    while(1) {
        Shell();
    }
}