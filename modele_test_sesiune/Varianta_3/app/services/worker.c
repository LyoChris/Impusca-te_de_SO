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

int IsTrig(char trig[]) {
    if(strchr("aeiouAEIOU", trig[0]) != NULL) return 0;
    if(strchr("aeiouAEIOU", trig[1]) == NULL) return 0;
    if(strchr("aeiouAEIOU", trig[2]) != NULL) return 0;
    return 1;
}

void trigs_select(int fd_fifo) {
    char set[4];
    while(read(STDIN_FILENO, &set, sizeof(set)) == sizeof(set)) {
        if(IsTrig(set) == 1) write(fd_fifo, &set, sizeof(set));
    }

    close(fd_fifo);
}

int main(int argc, char*argv[]) {
    int fd_fifo;
    if((fd_fifo = open("./fifosw", O_WRONLY)) == -1) {
        perror("Eroare la open fifo");
        exit(1);
    }

    trigs_select(fd_fifo);

    return 0;
}