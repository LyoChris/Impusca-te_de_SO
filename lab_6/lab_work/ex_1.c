#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>   // necesar pentru variabila errno si constantele simbolice asociate codurilor numerice de eroare
#include <limits.h>  // necesar pentru PATH_MAX
#include <stdbool.h>

//for easy commands:
//  gcc -Wall -o ex_1 ex_1.c
// ./ex_1 /home/lyo/Desktop/Facultate/Exemple_C/lab_6/ex_1_input.txt /home/lyo/Desktop/Facultate/Exemple_C/lab_6/ex_1_output.txt a b

#define BUFF_SIZE 1024

int verif_existenta(char *filename){
    return access(filename, F_OK) != -1;
}

int confirm_overwrite(char *filename){
    char response;
    printf("Fisierul %s exista deja, doriti sa overwrite? (y/n) ", filename);
    scanf(" %c", &response);
    return (response == 'y' || response == 'Y');
}

int main(int argc, char* argv[]){

    char* input_file;
    char* output_file;
    char old_char;
    char new_char;
    int input_fd = 0;
    int output_fd = 0;

    if(argc == 4){
        input_file = argv[1];
        output_file = argv[1];
        old_char = argv[2][0];
        new_char = argv[3][0];
    }
    else {
        if(argc > 6){
            fprintf(stderr, "Argumente prea multe!\n");
            return 1;
        }
        else {
            input_file = argv[1];
            output_file = argv[2];
            old_char = argv[3][0];
            new_char = argv[4][0];
        }
    }

    if(!verif_existenta(input_file)){
        printf("Fisierul input nu exista\n");
        return 2;
    }

    input_fd = open(input_file, O_RDONLY);
    if(input_fd == -1){
        perror("Eroare la deschiderea fisierului de intrare");
        return 1;
    }

    if (strcmp(input_file, output_file) != 0) {
        if(verif_existenta(output_file)){
            if(!confirm_overwrite(output_file)){
                printf("Suprascriere anulata!\n");
                return 0;
            }
        }
    }

    if (strcmp(input_file, output_file) == 0) {
        output_fd = open(input_file, O_RDWR);
        if (output_fd == -1) {
            perror("Eroare la deschiderea fișierului de intrare pentru citire și scriere");
            close(input_fd);
            return 1;
        }
    }
    else {
        
        output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if(output_fd == -1){
            perror("Can't create/open output file");
            close(input_fd);
            return 1;
        }
    }

    char buffer[BUFF_SIZE];
    //ssize_t bytes_read;
    int bytes_read;

    while((bytes_read=read(input_fd, buffer, BUFF_SIZE)) > 0) {
        for(int i = 0; i < bytes_read; i++) {
                if(buffer[i] == old_char) {
                    buffer[i] = new_char;
                }
        }

        if (strcmp(input_file, output_file) == 0) {
            lseek(output_fd, 0, SEEK_SET);
        }

        if(write(output_fd, buffer, bytes_read) == -1) {
            perror("Eroare la scrierea în fișierul de ieșire");
            close(input_fd);
            close(output_fd);
            return 1;
        }
    }

    if (bytes_read == -1) {
        perror("Eroare la citirea fișierului de intrare");
    }

    close(input_fd);
    close(output_fd);

    printf("Fișierul %s a fost procesat cu succes.\n", output_file);
    return 0;


}