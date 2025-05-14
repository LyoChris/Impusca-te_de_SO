#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>    // necesar pentru functia getpwuid
#include <grp.h>    // necesar pentru functia getgrgid
#include <limits.h> // necesar pentru PATH_MAX

#define PATH 4096

void parcurgere_fisiere(char* cale_cur, char* fisier_cautat) {
    DIR *dir = opendir(cale_cur);
    if (dir == NULL) {
        perror("Eroare parcurgere fisiere!");
        exit (1);
    }

    printf("123\n");

    struct dirent* entry;
    struct stat info;

    while((entry =  readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
    }
    printf("123\n");
    if (strcmp(entry->d_name, fisier_cautat) == 0) {
        fprintf(stdin,"%s/%s\n", cale_cur, entry->d_name);
    }
    printf("123\n");

    char cale_completa[PATH_MAX];
    printf("123\n");
    sprintf(cale_completa,"%s/%s", cale_cur, entry->d_name);
    printf("123\n");
    if(stat(cale_completa, &info) == 0 && S_ISDIR(info.st_mode)){
        printf("123\n");
        parcurgere_fisiere(cale_completa, fisier_cautat);
    }

    closedir(dir);

}

int main(int argc, char* argv[]) {
    if (argc != 2){
        fprintf(stderr, "Utilizare: %s <nume_fisier>\n", argv[0]);
        exit (1);
    }

    printf("123\n");
    char* home_dir = getenv("HOME");
    if(home_dir == NULL) {
        fprintf(stderr, "Eroare la obtinerea directorului home\n");
        exit (1);
    }
    printf("123\n");

    parcurgere_fisiere(home_dir, argv[1]);

    return 0;
}

