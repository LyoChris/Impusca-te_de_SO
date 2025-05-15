#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>

int main(int argc, char* argv[]) {
    unsigned int i;
    int fd;
    float n, nr;
    int cale;
    char nume_fisier[PATH_MAX];
    scanf("%d", &cale);
    fd=open(nume_fisier, O_RDWR);

    if(cale == 0 ) {
    if(argc == 1) {
        printf("Introduceti numele fisierului ce se va crea/suprascrie: ");
        scanf("%s", nume_fisier);
    } else {
        strcpy(nume_fisier, argv[1]);
    }

    fd = open(nume_fisier, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Eroare la crearea fisierului de date");
        exit(1);
    }

    printf("Introduceti lungimea secventei de numere: ");
    if (scanf("%f", &n) != 1) {
        fprintf(stderr, "Error: incorrect format for an integer number!\n");
        exit(EXIT_FAILURE);
    }

    if (n <= 0) {
        fprintf(stderr, "Error: you need to input a strictly positive integer number!\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < n; i++) {
        printf("Introduceti al %d-lea numar: ", i);
        if (scanf("%f", &nr) != 1) {
            fprintf(stderr, "Error: incorrect format for an integer number!\n");
            exit(EXIT_FAILURE);
        }

        if (write(fd, &nr, sizeof(float)) == -1) {
            perror("Eroare la scrierea in fisierul de date");
            exit(2);
        }
    }
}

    lseek(fd, 0, SEEK_SET);

    int rcod;
    printf("Secventa citita din fisier: \n");
    while (1) {
        rcod = read(fd, &nr, sizeof(float));
        if (rcod == -1) {
            perror("Eroare la citirea din fisierul de date");
            exit(2);
        }

        if (rcod == 0) {
            break;
        }

        printf("%f , ", nr);
    }

    close(fd);

    return 0;
}
