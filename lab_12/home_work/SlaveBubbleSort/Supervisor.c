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
#include <signal.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <limits.h>

int file_creator(char* filename) {
    int fd;
    if((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) {
        perror("Eroare la deschidere fisier");
        exit(1);
    }
    return fd;
}

int file_opener(char* filename) {
    int fd;
    if((fd = open(filename, O_RDWR)) == -1) {
        perror("Eroare la deschidere fisier");
        exit(1);
    } 
    return fd;
}

off_t get_file_size(int fd) {
    off_t current_pos = lseek(fd, 0, SEEK_CUR); //salvez curent position
    off_t file_size = lseek(fd, 0, SEEK_END); //merg la final
    lseek(fd, current_pos, SEEK_SET); // revin la positia intiala
    return file_size; 
}

void read_numbers(int fd) {
    lseek(fd, 0, SEEK_SET);
    int entries = get_file_size(fd) / sizeof(int);
    for(int i = 0; i < entries; i++) {
        int a;
        read(fd, &a, sizeof(int));
        printf("%d ", a);
    }
    printf("\n");
}

void write_random_numbers(int fd) {
    srand(getpid());
    int n = rand() % 20;
    for(int i = 0; i < n; i++) {
        int a = rand() % 1000;
        write(fd, &a, sizeof(int));
    }
    lseek(fd, 0, SEEK_SET);
    printf("Numerele scrie in fisier sunt: ");
    read_numbers(fd);
}

int main(int argc, char* argv[]) {
    int n;
    if(argc != 2) {
        printf("Dati numarul de instante: ");
        scanf(" %d", &n);
    }
    else {
        n = atoi(argv[1]);
        if(n < 0) {
            printf("Numarul de instante trebuie sa fie pozitiv\n");
            exit(2);
        }
    }

    pid_t pids[n];
    int count = 0;
    int fd = file_creator("./secventa.bin");
    write_random_numbers(fd);

    for(int i = 0; i < n; i++) {
        pid_t pid;
        if((pid = fork()) == -1) {
            perror("Eroare la fork");
            exit(3);
        }
        if(pid == 0) {
            execlp("/home/lyo/Desktop/Facultate/SO/lab_12/home_work/SlaveBubbleSort/cacaturi_din_lab_7/SorterSlave", "Sorter Slave", "secventa.bin", NULL);

            perror("Eroare la execlp");
            exit(4);
        }
        else {
            pids[count++] = pid;
        }
    }
    for(int i = 0; i < n; i++) {
        int codterm;
        wait(&codterm);
        if((codterm >> 8) != 0) {
            printf("NU am reusit sa sortez\n");
            exit(5);
        }
        else {
            for(int i = 0; i < n; i++) {
                kill(pids[i], SIGTERM);
            }
        }
    }

    printf("Secventa sortata: ");
    read_numbers(fd);
    close(fd);
}