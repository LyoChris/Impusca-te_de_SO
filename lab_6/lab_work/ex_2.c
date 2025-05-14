#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>   // necesar pentru variabila errno si constantele simbolice asociate codurilor numerice de eroare
#include <limits.h>  // necesar pentru PATH_MAX

#define BUFF_SIZE 1024

void print(FILE *file,  int lines, int chars){
    char buffer[BUFF_SIZE];
    int count = 0;

    if(lines != -1) {
        while(lines > 1 && fgets(buffer, BUFF_SIZE, file)) {
            printf("%s", buffer);
            lines--;
        }
    }
    else { 
        if (chars != -1) {
            while (chars > 0 && (count = fgetc(file)) != EOF) {
                putchar(count);
                chars--;
            }
            
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]){
    int opt;
    int lines = -1, chars = -1;
    int file_arg_start = 1;
    while ((opt = getopt(argc, argv, "n:c:")) != -1){
        switch(opt) {
            case 'n':
                lines = atoi(optarg);
                break;
            case 'c':
                chars = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Utilizare: %s [-n numar_linii] [-c numar_caractere] fisier1 fisier2 ...\n", argv[0]);
                return 1;
        }
    }

    if (lines == -1 && chars == -1) lines = 10;

    if (optind == argc){
        print(stdin, lines, chars);
    }
    else {
        for (int i = optind; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if (!file) {
                perror("Eroare la deschiderea fișierului");
                continue;
            }

            printf("\n==> %s <==\n", argv[i]);
            print(file, lines, chars);
            fclose(file);
        }
    }

}