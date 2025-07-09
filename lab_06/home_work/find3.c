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

int nr=0;

void del_links(char* cale_cur) {
    struct stat st;
    char target[PATH_MAX];

    if(0 != lstat(cale_cur, &st)) {
        return;
    }
    if(S_ISLNK(st.st_mode)) {
        size_t len = readlink(cale_cur, target, sizeof(target) - 1);
        if (len == -1) {
            return;
        }
        
        target[len] = '\0';
        if(access(target, F_OK) == -1) {
                if(unlink(cale_cur) == -1) {
                    perror("Eroare la ștergerea legăturii simbolice");
                } 
                else {
                    printf("Legătura simbolică ruptă \"%s\" a fost ștearsă.\n", cale_cur);
                }
                nr++;
        }
    }

}

int prop_file(char* cale_cur) {
    struct stat st;
    int result=0;

    if(0 != stat(cale_cur,&st) ) {
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        return 1;
    }

    return -1;
}

void parcurgere_dir(char* cale_cur) {
    DIR *dir;
    struct dirent *de;

    char file_name[PATH_MAX];
    int is_Folder;

    is_Folder= prop_file(cale_cur);
    if(is_Folder == -1){
        return;
    }
    else {
        if(NULL == (dir = opendir(cale_cur))) {
            return;
        }
        while(NULL != (de = readdir(dir))) {
            if(de->d_name[0] != '.' && strcmp(de->d_name,".") && strcmp(de->d_name,"..")) {
                sprintf(file_name,"%s/%s",cale_cur,de->d_name);
                del_links(file_name);
                parcurgere_dir(file_name);
            }
        }

        closedir(dir);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2){
        fprintf(stderr, "Utilizare: %s <nume_fisier>\n", argv[0]);
        exit (1);
    }

    char* cale = argv[1];
    parcurgere_dir(cale);
}

