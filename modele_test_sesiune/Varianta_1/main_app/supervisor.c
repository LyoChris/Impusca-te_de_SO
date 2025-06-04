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
#include <semaphore.h>
#include <limits.h>

#define FIFO "./fifo1"

int counter = 0;

bool IsSeparator(char ch) {
    return ch == ' ' || ch == ',' || ch == '\n' || ch == '?' || ch == '\t'; 
}

void numbers_to_worker1(char* file, int fd_pipe) {
    int fd;
    if((fd = open(file, O_RDONLY)) == -1) {
        perror("Eroare la deschidere fisier");
        exit(1);
    }
    
    char number[100], ch;
    int count = 0;

    while(read(fd, &ch, sizeof(char)) > 0) {
        if(IsSeparator(ch)) {
            if(count > 0) {
                number[count] = '\0';
                int num = atoi(number);
                write(fd_pipe, &num, sizeof(int));
                counter++;
            }
            count = 0;
        }
        else {
            number[count++] = ch;
        }
    }

    if(count > 0) {
        number[count] = '\0';
        int num = atoi(number);
        write(fd_pipe, &num, sizeof(int));
        counter++;
    }

    close(fd_pipe);

}

void numbers_from_worker1(char* file) {
    int sum = 0, fd, num = 0;

    if((fd = open(FIFO, O_RDONLY)) == -1) {
        perror("Eroare la deschidere fifo in sup");
        exit(1);
    }

    int cont;
    read(fd, &sum, sizeof(int));
    read(fd, &cont, sizeof(int));

    char number[25];
    sprintf(number, "%d", sum);
    num = 0;

    for(int i = 0; i < strlen(number); i++) {
        num += (number[i] - '0');
    }

    printf("Suma cifrelor este: %d si contorul de nr impare este %d\n", num, cont);

    close(fd);

}

int main(int argc, char* argv[]) {
    // char cwd[PATH_MAX];
    // getcwd(cwd, sizeof(cwd));
    // printf("%s\n", cwd);

    if(argc != 2) {
        fprintf(stderr, "Usage: %s [FISIER INPUT] \n", argv[0]);
        exit(1);
    }

    int pipesto1[2];
    if (pipe(pipesto1) == -1) {
        perror("Eroare la creare pipe");
        exit(2);
    }
    
    if(mkfifo(FIFO, 0600) == -1) {
        if(errno == 17) { }
        else {
            perror("Eroare la creare fifo");
            exit(3);
        }
    }

    pid_t pid;
    if((pid = fork()) == -1) {
        perror("Eroare la creeare fiu");
        exit(4);
    }
    if(pid == 0 ) {
        close(pipesto1[1]);
        dup2(pipesto1[0], STDIN_FILENO);
        close(pipesto1[0]);
        
        execlp("./main_app/procesators/worker1", "worker1", NULL);

        perror("Eroare la exec worker1");
        exit(5);
    }
    else {
        close(pipesto1[0]);

        numbers_to_worker1(argv[1], pipesto1[1]);

        //wait(NULL);

        numbers_from_worker1(FIFO);
    }

    return 0;
}