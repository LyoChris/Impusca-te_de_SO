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
#include <semaphore.h>

typedef struct optiune {
    int l, w, c, L;
    int index;
    int total_l, total_w, total_c, total_L;
} Optiuni;

int numbers_of_files = 0;

Optiuni prelucrare_opt(int argc, char* argv[]) {
    Optiuni opt = {0, 0, 0 ,0, 0, 0, 0, 0, 0};
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-l") == 0){
            opt.l = 1;
            opt.index++;
        }
        if(strcmp(argv[i], "-w") == 0){
            opt.w = 1;
            opt.index++;
        }
        if(strcmp(argv[i], "-c") == 0) {
            opt.c = 1;
            opt.index++;
        }
        if(strcmp(argv[i], "-L") == 0){
            opt.L = 1;
            opt.index++;
        }
    }

    if(opt.l == 0 && opt.w == 0 && opt.c == 0 && opt.L == 0) {
        opt.l = 1;
        opt.w = 1;
        opt.c = 1;
    }

    return opt;
}

int opt_l(char* mmap_in, int size) {
    int count = 0;
    for(int i = 0; i < size; i++) {
        if(*(mmap_in + i) == '\n') count++;
    }

    printf(" %d ", count);
    return count;
}

int opt_w(char* mmap_in, int size) {
    int count = 0;
    int in_word = 0;

    for (int i = 0; i < size; ++i) {
        if (isspace((unsigned char)mmap_in[i])) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
    }
    
    printf(" %d ", count);
    return count;
}

int opt_L(char* mmap_in, int size) {
    int max_line = -1;
    int length = 0;

    for(int i = 0; i < size; i++) {
        length++;
        if(mmap_in[i] == '\n') {
            if(length > max_line) {
                max_line = length;
            }
            length = 0;
        }
    }

    printf(" %d ", max_line);
    return max_line;
}

void wc(char* filename, Optiuni* opt, int index, int argc) {
    struct stat st;
    int fd;
    if((fd = open(filename, O_RDONLY)) < 0) {
        perror("Eroare la deschidere");
        exit(1);
    }

    if((fstat(fd, &st)) < 0) {
        perror("Eroare la fstat");
        exit(2);
    }

    char* mmap_in = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(mmap_in == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(3);
    }

    close(fd);

    if(opt->l == 1) {
        opt->total_l += opt_l(mmap_in, st.st_size);
    }
    if(opt->w == 1) {
        opt->total_w += opt_w(mmap_in, st.st_size);
    }
    if(opt->c == 1) {
        printf(" %ld ", st.st_size);
        opt->total_c += st.st_size;
    }
    if(opt->L == 1) {
        opt->total_L += opt_L(mmap_in, st.st_size);
    }
    printf(" %s\n", filename);
    numbers_of_files++;

    if(argc == (index + 1) && numbers_of_files != 1) {
        if(opt->total_l != 0){
            printf(" %d ", opt->total_l);
        }
        if(opt->total_w != 0){
            printf(" %d ", opt->total_w);
        }
        if(opt->total_c != 0){
            printf(" %d ", opt->total_c);
        }
        if(opt->total_L != 0){
            printf(" %d ", opt->total_L);
        }
        printf(" total\n");
    }
}

int main(int argc, char* argv[]) {
    Optiuni opt = prelucrare_opt(argc, argv);
    for(int i = opt.index+1; i < argc; i++) {
        wc(argv[i], &opt, i, argc);
    }

}