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
        fprintf(stderr, "Not enough arguments!\n");
        exit(1);
    }

    if(strchr(argv[1], '+')) {
        fprintf(stderr, "Numele fisierul %s nu poate contine caracterul '+'.\n", argv[1]);
        exit(3);
    }
    struct stat st;
    if(stat(argv[1], &st) != 0) {
        perror("Eroare la citirea fisierului");
        exit(2);
    }

    off_t size = st.st_size;
    float pag_numb = size/4096.0;
    int numb = 0;
    
    if( pag_numb != (int)pag_numb ) numb = (int)pag_numb + 1;
    else
        numb = (int)pag_numb;

    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        perror("Eroare la deschiderea fisierului");
        exit(3);
    }

    int count = 0;
    char buffer[BUFF_SIZE];
    int bytes = 0;
    char s[2];

    while (count < numb) {
        bytes = read(input_fd, buffer, BUFF_SIZE);
        if (bytes <= 0) {
            break;
        }
        if (count + bytes >= numb) {
            s[0] = buffer[numb - count - 1];
            break;
        }

        count += bytes;
    }

    printf("%d+++%s+++%d\n", numb, argv[1], (int)s[0]);

}