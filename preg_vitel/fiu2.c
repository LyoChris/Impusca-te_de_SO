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

int main() {
    int readfd, shm_fd, i = 0;

    readfd = open("./fifo1", O_RDONLY);

    shm_fd = shm_open("/shm_2", O_RDWR, 0);

    int* map = mmap(NULL, 100*sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);

    int number;
    while(read(readfd, &number, sizeof(int)) != 0) {
        printf("[FIU 2]: Citit numarL %d.\n", number);
        map[i++] = number * number;
    }

    map[i] = -1;

    close(readfd);
    return 0;
}