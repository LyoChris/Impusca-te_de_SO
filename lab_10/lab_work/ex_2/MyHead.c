#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct Optiune {
    char opt;
    int val;
    int index;
} Optiune;

Optiune processing_option(int argc, char* argv[]) {
    Optiune a = {'n', 10, 1};
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <options> <file name 1> ... \n", argv[0]);
        exit(1);
    }
    for (int i = 1; i < argc - 1; i++) {
        if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "-c") == 0) && isdigit(argv[i+1][0])) {
            a.opt = argv[i][1];
            a.val = atoi(argv[i+1]);
            a.index = i + 1;
            i++;
        }
    }

    return a;
}

void writer(char* filename, Optiune o) {
    int input_fd = 0;
    if((input_fd = open(filename, O_RDONLY)) == -1) {
        perror("Eroare la open");
        exit(2);
    }

    struct stat sb;
    if(stat(filename, &sb) == -1) {
        perror("Eroare la stat");
        exit(3);
    }

    char* mmap_in = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, input_fd, 0);
    if(mmap_in == MAP_FAILED) {
        perror("Eroare la map");
        exit(4);
    }

    if(close(input_fd) == -1) {
        perror("Eroare la grila");
        exit(5);
    }

    printf("\n======== %s ========\n", filename);
    int counter = 0;
    for(int i = 0; i < sb.st_size; i++) {
        if(o.opt == 'n') {
            if(*(mmap_in + i) == '\n') counter++;
            write(STDOUT_FILENO, mmap_in + i, 1);
            if(counter > o.val){
                printf("\n");
                return;
            }
        }
        if(o.opt == 'c') {
            counter++;
            write(STDOUT_FILENO, mmap_in + i, 1);
            if(counter > o.val - 1) {
                printf("\n");    
                return;
            }
        }
    }

    if (-1 == munmap(mmap_in, sb.st_size) ) {
        perror("Erorr at munmap");
        exit(6);
    }

}


int main(int argc, char* argv[]) {
    Optiune o = processing_option(argc, argv);

    for(int i=o.index + 1; i<argc; i++) {
        writer(argv[i], o);
    }
}