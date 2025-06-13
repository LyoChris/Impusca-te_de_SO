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

int main(int argc, char* argv[]) {
    FILE* numeberfd;
    int pipe1[2];
    int number, shm_fd;
    pid_t fiu1,fiu2;

    numeberfd = fopen(argv[1], "r");
    if(numeberfd == NULL) {

    }

    pipe(pipe1); //creez pipe dintre parinte si fiu 1

    if(mkfifo("./fifo1", 0600) == -1) {
        if(errno == EEXIST) {}
    }

    shm_fd = shm_open("/shm_2", O_CREAT | O_RDWR, 0600);

    ftruncate(shm_fd, 100*sizeof(int));

    int* map = mmap(NULL, 100*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    fiu1 = fork(); //creez primul fiu
    if(fiu1 == 0) {
        close(pipe1[1]); //inchid capatul de scriere din fiu, el doar citeste
        
        int fd_fifo;
        fd_fifo = open("./fifo1", O_WRONLY);

        int num;
        while(read(pipe1[0], &num, sizeof(int)) == sizeof(int)) 
        {
            printf("[FIU1]: Am citit numarul %d.\n", num);
            num /= 2;
            write(fd_fifo, &num, sizeof(int));
            printf("[FIU1]: Am scris numarul %d.\n", num);
        }

        close(fd_fifo);
        close(pipe1[0]); //inchid capatul de citire in fiu
        return 0;
    }

    fiu2 = fork();
    if(fiu2 == 0) {
        close(pipe1[0]);
        close(pipe1[1]);
        execlp("./fiu2", "fiu2", NULL);
        return 1;
    }

    close(pipe1[0]); //inchid capatul de citire din parinte, el doar scrie

    while(fscanf(numeberfd, "%d", &number) == 1) 
    {
        write(pipe1[1], &number, sizeof(int)); //scriem numarul in pipe
        printf("[PARINTE]: Am scris numarul %d.\n", number);
    }

    close(pipe1[1]); //am terminat scrierea in pipe, inchidem capatul de scriere AKA punem EOF

    sleep(2);

    for(int i = 0; map[i] != -1; i++){
         printf("[PARINTE]: Am primit din mapare numarul: %d.\n", map[i]);
    }

    
    wait(NULL);
    wait(NULL);

    munmap(map, 100*sizeof(int));
    fclose(numeberfd);
    return 0;
}