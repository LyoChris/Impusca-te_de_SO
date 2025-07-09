#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>    // necesar pentru functia getpwuid
#include <grp.h>    // necesar pentru functia getgrgid
#include <limits.h> // necesar pentru PATH_MAX

#define BUFF_SIZE 4096

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Improper use");
        exit(1);
    }

    int input_fd=open(argv[1], O_RDONLY);
    if(input_fd == -1) {
        perror("Eroare la deschiderea fisierului:");
        exit(2);
    }

    char buff[BUFF_SIZE];
    int bytes_read = 0;
    int ok = 0;
    int first, last;

    while((bytes_read = read(input_fd, buff, BUFF_SIZE)) > 0) {
        if(ok == 0) {
            first = (int)buff[0];
            ok = 1;
        }
        last = (int)buff[bytes_read - 2];
    }

    if(ok == 0){
        first = 0;
        last = 0;
    }

    if(bytes_read == -1) {
        perror("Eroare la citire din fisier");
        close(input_fd);
        exit(2);
    }

    printf("%s=%d\n", argv[1], (first +last));

}