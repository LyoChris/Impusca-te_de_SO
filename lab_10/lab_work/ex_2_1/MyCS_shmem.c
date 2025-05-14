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

#define FILE_NAME "peco.bin"
#define SEM_NAME "/peco_mutex"

void initializare_fisier_date() {
    int fd;
    float val;

    printf("Cantitatea de start in rezervorul PECO-ului: ");
    while ((1 != scanf("%f", &val)) || (val < 0)) {
        fprintf(stderr, "Eroare: introduceti un numar real pozitiv.\n");
        while (getchar() != '\n');
    }

    if((fd = creat(FILE_NAME, 0600)) == -1) {
        perror("Eroare la creare fisier");
        exit(1);
    }

    if (write(fd, &val, sizeof(float)) == -1) {
        perror("Eroare la scrierea valorii initiale"); exit(2);
    }

    close(fd);
    printf("Rezervor initializat cu %.2f litri.\n", val);
}

void afisare_date() {
    int fd;
    float val;

    fd = open(FILE_NAME, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la deschiderea fisierului"); exit(3);
    }

    if (read(fd, &val, sizeof(float)) == -1) {
        perror("Eroare la citirea valorii"); exit(4);
    }

    close(fd);
    printf("Stoc final: %.2f litri de combustibil.\n", val);
}

void secventa_actualizari(int argc, char* argv[]) {
    float valoare, *stoc;
    int fd, i;
    sem_t *mutex;

    if((fd = open(FILE_NAME, O_RDWR)) == -1) {
        perror("Eroare la deschiderea fisierului");
        exit(5);
    }

    stoc = mmap(NULL, sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(stoc == MAP_FAILED) {
        perror("Eroare la map");
        exit(6);
    }

    mutex = sem_open(SEM_NAME, O_CREAT, 0600,1);
    if (mutex == SEM_FAILED) {
        perror("Eroare la sem_open"); exit(7);
    }

    srandom(getpid());

    for(i = 1; i < argc; i++) {
        if(sscanf(argv[i],"%f", &valoare) != 1) {
            fprintf(stderr, "Argument invalid: %s\n", argv[i]);
            continue;
        }

        sem_wait(mutex);

        float nou_stoc = *stoc + valoare;

        if(nou_stoc < 0) {
            printf("[PID %d] Cerere: %.2f, dar stocul este %.2f.\n", getpid(), valoare, *stoc);
            printf("Alegeți opțiunea:\n1 - Refuză operația\n2 - Extrage parțial (%.2f litri)\n", *stoc);
            int opt;
            printf("Introdu opțiunea (1 sau 2): ");
            fflush(stdout);

            while (scanf("%d", &opt) != 1 || (opt != 1 && opt != 2)) {
                printf("Opțiune invalidă. Introdu 1 sau 2: ");
                while (getchar() != '\n');
            }

            if (opt == 1) {
                printf("[PID %d] Operația refuzată de utilizator.\n", getpid());
            } else {
                printf("[PID %d] Se extrag %.2f litri (tot stocul). Stoc devine 0.\n", getpid(), *stoc);
                *stoc = 0.0f;
            }
        } else {
            printf("[PID %d] Modificare stoc: %.2f -> %.2f\n", getpid(), *stoc, nou_stoc);
            *stoc = nou_stoc;
        }

        sem_post(mutex);

        sleep(random() % 5);
    }

    munmap(stoc, sizeof(float));
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Utilizare: %s -i | -o | <valori>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-i") == 0) {
        initializare_fisier_date();
        return 0;
    }

    if (strcmp(argv[1], "-o") == 0) {
        afisare_date();
        return 0;
    }

    secventa_actualizari(argc, argv);
    return 0;
}