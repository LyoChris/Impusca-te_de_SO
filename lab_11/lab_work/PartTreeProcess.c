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


void afis_info(int i, int j, pid_t ppid, int n_chld, pid_t childs[], int exit[]){
    printf("(%d,%d): PID=%d, PPID=%d", i, j, getpid(), ppid);
    if(n_chld > 0) {
        printf(", children: ");
        for (int k = 0; k < n_chld; k++) {
            printf("[PID=%d, exit_code=%d] ", childs[k], exit[k]);
        }
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    int n = 3, m = 4;
    pid_t pid;
    pid_t child_l2[m];
    int exit_l2[m];

    printf("(1,1): PID=%d, PPID=%d\n", getpid(), getppid());

    for(int i = 1; i <= m; i++) {
        if((pid = fork()) == -1) {
            handle_err("Eroare creare fiu");
        }
        if(pid == 0) {
            pid_t child_l3[n];
            pid_t exit_l3[n];
            for(int j = 0; j < n; j++) {
                if((pid = fork()) == -1) {
                    handle_err("Eroare creare fiu");
                }
                if(pid == 0) {
                    afis_info(3, (i* n + j + 1), getppid(), 0, NULL, NULL);
                    exit((i* n + j + 1));
                }
                else {
                    child_l3[j] = pid;
                }
            }
            for (int j = 0; j < n; j++) {
                int status;
                waitpid(child_l3[j], &status, 0);
                if (WIFEXITED(status))
                    exit_l3[j] = WEXITSTATUS(status);
                else
                    exit_l3[j] = -1;
            }

            afis_info(2, i + 1, getppid(), n, child_l3, exit_l3);
            exit((100 + i));
        } else {
            child_l2[i] = pid;
        }

    }

    for (int i = 0; i < m; i++) {
        int status;
        waitpid(child_l2[i], &status, 0);
        if (WIFEXITED(status))
            exit_l2[i] = WEXITSTATUS(status);
        else
            exit_l2[i] = -1;
    }

    afis_info(1, 1, getppid(), m, child_l2, exit_l2);
    
    return 0;
}