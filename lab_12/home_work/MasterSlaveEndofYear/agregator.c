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

typedef struct vrajeala {
    int a, b, c;
} report;

#define NAMES_MAX 1024

void parcurgere_dir_luna(char* cale_cur, char (*shm_names)[NAMES_MAX]) {
    int i = -1;
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
        
        i++;
        char cale_completa[PATH_MAX];
        sprintf(cale_completa,"%s/%s", cale_cur, entry->d_name);
        pid_t pid;
        if((pid = fork()) == -1) {
            perror("eroare la fork");
            exit(3);
        }
        if(pid == 0) {
            execlp("./slave", "slave", cale_completa, shm_names[i], NULL);

            perror("eroare la execlp");
            exit(4);
        }
        

    }
   
    for (int i = 0; i < 12; i++) {
        wait(NULL);
    }
    closedir(dir);
}

int main(int argc, char* argv[]) {
    char shm_names[12][NAMES_MAX];

    for (int i = 0; i < 12; i++) {
        snprintf(shm_names[i], sizeof(shm_names[i]), "/shm_aggr_%d", i);
        int fd;
        if ((fd = shm_open(shm_names[i], O_CREAT|O_RDWR, 0600)) == -1) {
            perror("eroare la shm_open");
            exit(1);
        }
        if (ftruncate(fd, 3 * sizeof(int)) < 0) {
            perror("eroare la ftruncate");
            exit(1);
        }
        close(fd);
    }

    parcurgere_dir_luna(argv[1], shm_names);

    report final;

    for(int i = 0; i < 12; i++) {
        int fd;
        if ((fd = shm_open(shm_names[i], O_RDWR, 0)) == -1) {
            perror("eroare la shm_open");
            exit(1);
        }
        int a, b, c;
        read(fd, &a, sizeof(int));
        read(fd, &b, sizeof(int));
        read(fd, &c, sizeof(int));
        final.a+=a;
        final.b+=b;
        final.c+=c;
        printf("Sumele doar pana in luna %d sunt: %d, %d, %d\n", i, final.a, final.b, final.c);
        close(fd);
    }

    printf("Sumele finale sunt: %d, %d, %d\n", final.a, final.b, final.c);

    return 0;

}