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
#include <math.h>
#include <libgen.h>
#include <semaphore.h>
#include <limits.h>

void path_to_worker(char* argv, char* path) {
    char resolved[PATH_MAX];
    if(realpath(argv, resolved) == NULL) {
        perror("Eroare la realpath");
        exit(3);
    }
    dirname(resolved);
    sprintf(path, "%s/%s", resolved, "worker1");
}

void sex_work1(int fd_fifo, int fd_pipe) {
    int contor = 0;
    char ch;
    while(read(fd_fifo, &ch, sizeof(char)) > 0) {
        if(isupper(ch)) contor++;
        char c = tolower(ch);
        write(fd_pipe, &c, sizeof(char));
    }

    write(fd_pipe, "$", sizeof("$"));
    write(fd_pipe, &contor, sizeof(contor));

    close(fd_fifo);
    close(fd_pipe);
}

int main(int argc, char* argv[]) {
    int fd_pipe[2], fd_fifo;
    if(pipe(fd_pipe) == -1) {
        perror("Eroare la pipe");
        exit(1);
    }

    if(mkfifo("./fifosw", 0600) == -1) {
        if(errno == EEXIST) {}
        else {
            perror("Eroare la mkfifo");
            exit(2);
        }
    }

    pid_t pid;
    if((pid = fork()) == -1) {
        perror("Eroare la fork");
        exit(1);
    }

    if(pid == 0) {
        char path[PATH_MAX];
        path_to_worker(argv[0], path);
        close(fd_pipe[1]);
        dup2(fd_pipe[0], STDIN_FILENO);
        close(fd_pipe[0]);

        execlp(path, "worker1", NULL);

        perror("Eroare la exec");
        exit(2);
    }
    else {
        close(fd_pipe[0]);

        if((fd_fifo = open("./fifosw", O_RDONLY)) == -1) {
            perror("Eroare la open fifo W");
            exit(6);
        }
        
        sex_work1(fd_fifo, fd_pipe[1]);

        return 0;
    }

}