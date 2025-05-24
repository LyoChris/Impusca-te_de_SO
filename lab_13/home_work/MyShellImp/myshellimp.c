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

#define handle_err() { \
    perror(error); exit(error)code; }

void exit_shell(int error) {
    switch (error) {
        case 1: printf("Eroare la citire, Shell se va inchide\n"); exit(1);
        case 2: exit(0);
        case 3: printf("Eroare la execvp, Shell se inchide\n"); exit(3);
        case 4: printf("Eroare la fork, Shell se inchide\n"); exit(4);
        case 5: printf("Eroare la pipe, Shell se va inchide"); exit(5);
        case 6: printf("Eroare la fork, Shell se va inchide"); exit(6);
        case 7: printf("Eroare la execvp, Shell se va inchide"); exit(7);
        default: error = 5; 
    }
}

void run_pipeline(char*** cmds, int count) {
    int (*pipes)[2] = malloc(sizeof(int[2]) * (count - 1));

    for(int i = 0; i < count - 1; i++) {
        if(pipe(pipes[i]) == -1) {
            perror("Eroare la pipe");
            exit_shell(5);
        }
    }

    for(int i = 0; i < count; i++) {
        pid_t pid;
        if((pid = fork()) == -1) {
            perror("Eroare la fork");
            exit_shell(6);
        }

        if(pid == 0) {
            if(i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO); //daca nu e primul pipe, 
                                                   //redirect stdin aka previous pipe read
            }

            if(i < count - 1) {
                dup2(pipes[i][1], STDOUT_FILENO); //daca nu e ultimul pipe, 
                                                   //redirect stdin aka curent pipe write
            }

            for(int j = 0; j < count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(cmds[i][0], cmds[i]);

            perror("Eroare la execvp");
            exit_shell(7);
        }
    }

    for (int i = 0; i < count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    free(pipes);

    for (int i = 0; i < count; i++) {
        int status;
        wait(&status);
    }
}

void ExecutePipe(char* line) {
    int pipes = 0;
    for (char *t = line; *t; t++)
        if (*t == '|') pipes++;
    int ncmd = pipes + 1;
    char *work = strdup(line);
    char **segments = malloc(ncmd * sizeof *segments);

    char *p = strtok(work, "|");
    for (int i = 0; i < ncmd; i++, p = strtok(NULL, "|")) {
        segments[i] = strdup(p);
    }
    free(work);

    char ***cmds = malloc(ncmd * sizeof *cmds);

    for (int i = 0; i < ncmd; i++) {
         int argc = 0;
        for (char *q = segments[i]; *q; ) {
            while (*q && isspace((unsigned char)*q)) q++;
            if (*q) {
                argc++;
                while (*q && !isspace((unsigned char)*q)) q++;
            }
        }
        cmds[i] = malloc((argc + 1) * sizeof *cmds[i]);

        int j = 0;
        char *tok = strtok(segments[i], " \t");
        while (tok) {
            cmds[i][j++] = tok;
            tok = strtok(NULL, " \t");
        }
        cmds[i][j] = NULL;
    }

    run_pipeline(cmds, ncmd);

    for (int i = 0; i < ncmd; i++) {
        free(segments[i]);
        free(cmds[i]);
    }
    free(segments);
    free(cmds);
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

    if(strchr(line, '|')){
        ExecutePipe(line);
        free(line);
        return;
    }
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