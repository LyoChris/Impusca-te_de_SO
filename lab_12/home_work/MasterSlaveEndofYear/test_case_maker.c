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
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <semaphore.h>
#include <limits.h>

//WARNING: asta e cod scris la 2 noaptea pe doua cafele si un corn, din 
//cauza ca un om nu imi da test cases umane, proceed at your own risk

int months = 12;
char* months_name[12] = {"ianuarie", "februarie", "martie", "aprilie", "mai", "iunie", "iulie", 
                        "august", "september", "octomber", "november", "december"};
char* database_in_plain_text;

int rand_negpos(int interval) {
    int range = 2 * interval + 1;
    return rand() % range - interval;
}

int rand_pos(int upper_limit) {
    return rand() % upper_limit;
}

void parcurgere_dir_luna(char* cale_cur, char* cale_database) {
    DIR *dir = opendir(cale_cur);
    if (dir == NULL) {
        perror("Eroare parcurgere fisiere!");
        exit (1);
    }

    struct dirent* entry;
    int fd1 = open(cale_database, O_WRONLY | O_APPEND);

    while((entry =  readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
    
        char cale_completa[PATH_MAX];
        sprintf(cale_completa,"%s/%s", cale_cur, entry->d_name);

        int fd = open(cale_completa, O_RDWR | O_TRUNC);
        int a = rand_negpos(100);
        int b = rand_pos(100);
        int c = rand_pos(100);
        write(fd, &a, sizeof(a));
        write(fd, &b, sizeof(b));
        write(fd, &c, sizeof(c));
        dprintf(fd1, "%d %d %d\n", a, b, c);
        write(fd1, "\n", sizeof("\n"));
        printf("Am scris in %s: %d, %d, %d.\n", cale_completa, a, b, c);
        close(fd);
    }
    close(fd1);
    closedir(dir);

}

void dau_foc_la_C5_et5(char* path, char* year) {
    char cale_completa[PATH_MAX];
    sprintf(cale_completa, "%s/%s", path, year);
    printf("%s\n", cale_completa);
    pid_t pid1 = fork();

    if(pid1 == 0) {
        execlp("mkdir", "mkdir", cale_completa, NULL);
    }

    wait(NULL);
    
    char cale_compl[PATH_MAX];
    sprintf(cale_compl, "%s/%s", cale_completa, "database_plain.txt");
    int fd = open(cale_compl, O_RDWR | O_CREAT | O_TRUNC, 0600);
    close(fd);

    for(int i = 0; i < months; i++) {
        pid1 = fork();
        if(pid1 == 0) {
            char cale_sex[PATH_MAX];
            sprintf(cale_sex, "%s/%s", cale_completa, months_name[i]);
            execlp("mkdir", "mkdir",  cale_sex, NULL);
        }
    }

    for(int i = 0; i < months; i++) {
        wait(NULL);
    }

    for(int i = 0; i < months; i++) {
        pid1 = fork();
        if(pid1 == 0) {
            srand(getpid());
            char* number_days[3] = {"29", "30", "31"};
            int r = rand() % 3;
            char cale_sex[PATH_MAX];
            sprintf(cale_sex, "touch %s/%s/{1..%s}", cale_completa, months_name[i], number_days[r]);
            execlp("bash", "bash", "-c", cale_sex, NULL);
        }
    }

    for(int i = 0; i < months; i++) {
        wait(NULL);
    }

    for(int i = 0; i < months; i++) {
        pid1 = fork();
        if(pid1 == 0) {
            srand(getpid());
            char cale_sex[PATH_MAX];
            sprintf(cale_sex, "%s/%s", cale_completa, months_name[i]);
            parcurgere_dir_luna(cale_sex, cale_compl);
            return i;
        }
        wait(NULL);
    }

}


int main(int argc, char* argv[]) {
    
    dau_foc_la_C5_et5(argv[1], argv[2]);
}
