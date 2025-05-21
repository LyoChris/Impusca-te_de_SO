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
    if(argc < 3) {
        fprintf(stderr, "Usage: %s [NUMBER OF PARALLEL PROCESSES] [NAME OF THE PARALLEL PROCESS] <arguemnts of parallel process\n", argv[0]);
        exit(1);
    }
    else {
        char **arg_for_exec = &argv[2];
        for(int i=0; i<atoi(argv[1]);i++) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                return EXIT_FAILURE;
            }
            if (pid == 0) {
                execvp(arg_for_exec[0], arg_for_exec);

                perror("Eroare la execvp");
                exit(10);
            }
        }
    }
}