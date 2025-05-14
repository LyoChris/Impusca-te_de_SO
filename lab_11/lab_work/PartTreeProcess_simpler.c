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

#define handle_err(err) \
    { perror(err); exit(EXIT_FAILURE); }

int main(int argc, char* argv[]) {
    int n = 3, m = 4;
    pid_t pid;

    printf("Sunt primul proces (%d,%d), cu PID-ul %d si parintele meu este procesul cu PID-ul %d.\n", 1, 1, getpid(), getppid());

    for (int i = 1; i <= m; i++) {
        if ((pid = fork()) == -1) {
            handle_err("Eroare creare fiu");
        }

        if (pid == 0) {
            printf("Sunt procesul fiu (%d,%d), cu PID-ul %d si parintele meu este procesul cu PID-ul %d\n", 2, i, getpid(), getppid());

            for (int j = 1; j <= n; j++) {
                if ((pid = fork()) == -1) {
                    handle_err("Eroare creare fiu");
                }

                if (pid == 0) {
                    printf("Sunt procesul fiu (%d,%d), cu PID-ul %d si parintele meu este procesul cu PID-ul %d\n", 3, (i - 1) * n + j, getpid(), getppid());
                    return i;
                }
            }

            for (int j = 1; j <= n; j++) {
                wait(NULL);
            }

            return i;
        }
    }

    
    for (int i = 1; i <= m; i++) {
        wait(NULL);
    }

    return 0;
}
