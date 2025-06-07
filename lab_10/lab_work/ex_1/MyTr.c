#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int request_overwrite(char* filename) {
    char response;
    while(response != 'y' && response != 'Y' && response != 'n' && response != 'N') {
        printf("Fisierul %s exista deja. Doriti sa overwrite? (y/n)  ", filename);
        scanf(" %c", &response);
    }
    if(response == 'Y' || response == 'y') return 1;
    return 0;
}

bool checker(char* filename) {
    if(access(filename, F_OK) == -1) return false;
    return true;
}

void replacer_diff_file(int input_fd, int output_fd, char replaced, char replacer) {
        char* mmap_input;
        char* mmap_output;
        struct stat sb_in;

        if (fstat(input_fd, &sb_in) == -1) {
            perror("Eroare la fstat");
            exit(3);
        }

        if(-1 == ftruncate(output_fd, sb_in.st_size)) {
            perror("Eroare la ftrunc");
            exit(4);
        }

        mmap_input = mmap(NULL, sb_in.st_size, PROT_READ, MAP_PRIVATE, input_fd, 0);
        if (mmap_input == MAP_FAILED) {
            perror("Eroare mmap input");
            exit(5);
        }

        mmap_output = mmap(NULL, sb_in.st_size, PROT_WRITE, MAP_SHARED, output_fd, 0);
        if (mmap_output == MAP_FAILED) {
            perror("Eroare mmap output");
            exit(5);
        }

        if (-1 == close(input_fd)) {
                perror("Eroare la close");
                exit(4);
        }
        if (-1 == close(output_fd)) {
            perror("Eroare la close");
            exit(4);
        }
        
        write(STDOUT_FILENO, mmap_input, sb_in.st_size);
        printf("\n");
        for(int i = 0; i < sb_in.st_size; i++) {
            if(mmap_input[i] == replaced) { //mmap_input[i]
                *(mmap_output + i) = replacer;
            }
            else {
                *(mmap_output + i) = *(mmap_input + i); 
            }
        }
        write(STDOUT_FILENO, mmap_output, sb_in.st_size);
        printf("\n");

        if (msync(mmap_output, sb_in.st_size, MS_SYNC) == -1) {
            perror("Erorr at mssync");
            exit(5);
        }

        if (-1 == munmap(mmap_input, sb_in.st_size) ) {
            perror("Erorr at munmap");
            exit(6);
        }

        if (-1 == munmap(mmap_output, sb_in.st_size) ) {
            perror("Erorr at munmap");
            exit(6);
        }
}

void replacer_same_file(int input_fd, char replaced, char replacer) {
    char* mmap_inout;
    struct stat sb_in;

    if (fstat(input_fd, &sb_in) == -1) {
        perror("Eroare la fstat");
        exit(3);
    }

    mmap_inout = mmap(NULL, sb_in.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, input_fd, 0);
    if (mmap_inout == MAP_FAILED) {
        perror("Eroare mmap input");
        exit(5);
    }
    if (-1 == close(input_fd)) {
            perror("Eroare la close");
            exit(4);
    }
    
    write(STDOUT_FILENO, mmap_inout, sb_in.st_size);
    printf("\n");
    for(int i = 0; i < sb_in.st_size; i++) {
        if(*(mmap_inout + i) == replaced) { //mmap_inout[i]
            *(mmap_inout + i) = replacer;
        }
    }
    write(STDOUT_FILENO, mmap_inout, sb_in.st_size);
    printf("\n");

    if (msync(mmap_inout, sb_in.st_size, MS_SYNC) == -1) {
        perror("Erorr at mssync");
        exit(5);
    }

    if (-1 == munmap(mmap_inout, sb_in.st_size) ) {
        perror("Erorr at munmap");
        exit(6);
    }
}

int main(int argc, char* argv[]) {
    char* input;
    char* output;
    char replacer, replaced;
    int input_fd = 0, output_fd = 0;
    if(argc == 5) {
        input = argv[1];
        output = argv[2];
        replaced = argv[3][0];
        replacer = argv[4][0];
    }
    else {
        if(argc == 4) {
            input = argv[1];
            output = argv[1];
            replaced = argv[2][0];
            replacer = argv[3][0];
        }
        else {
            printf("Usage: %s <input file> <output file(optional)> <characther replaced> <characther replacer> \n", argv[0]);
            return 1;
        }
    }

    if(!checker(input)) {
        printf("Fisierul input nu exista\n");
        return 2;
    }

    input_fd = open(input, O_RDONLY);
    if(input_fd == -1)
        perror("Eroare la deschiderea fisierului de intrare");
        return 1;
    

    if(strcmp(input, output) == 0) {
        if(checker(output)) {
            if(!request_overwrite(output)) {
                printf("Suprascriere anulata \n");
                return 0;
            }
        }
    }

    if(strcmp(input, output) == 0) {
        output_fd = open(output, O_RDWR);
        if (output_fd == -1) {
            perror("Eroare la deschiderea fișierului de intrare pentru citire și scriere");
            close(input_fd);
            return 1;
        }
    }
    else {
        output_fd = open(output, O_RDWR | O_CREAT | O_TRUNC, 0600);
        if(output_fd == -1){
            perror("Can't create/open output file");
            close(input_fd);
            return 1;
        }
    }

    printf("%s, %s \n", input, output);
    if(strcmp(input, output) == 0) {
        replacer_same_file(output_fd, replaced, replacer);
    }
    else {
        replacer_diff_file(input_fd, output_fd, replaced, replacer);
    }
}
