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


void numbers_to_worker2(int fifo_fd) {
    int num;
    int contor_inmultiri = 0;

    while(read(STDIN_FILENO, &num, sizeof(int)) > 0) {
        if(num % 3 != 0) {
            num = num * 3;
            contor_inmultiri++;
        }

        write(fifo_fd, &num, sizeof(int));
    }
    write(fifo_fd, &contor_inmultiri, sizeof(int));

    close(fifo_fd);

}

int main(int argc, char* argv[]) {

    if(mkfifo("./fifo", 0600) == -1) {
        if(errno == EEXIST) {}
        else {
            perror("eroare la fifo");
            exit(1);
        }
    }

    int fd_fifo;
    if((fd_fifo = open("./fifo", O_WRONLY)) == -1) {
        perror("Eroare la fifo");
        exit(2);
    }

    numbers_to_worker2(fd_fifo);
}