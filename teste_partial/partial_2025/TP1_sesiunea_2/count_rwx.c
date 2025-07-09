#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>    // necesar pentru functia getpwuid
#include <grp.h>    // necesar pentru functia getgrgid
#include <limits.h>

typedef struct {
    char masca[1024];
    int count;
} element;

void print_file(char* file) {
    int fd=open(file, O_RDONLY);
    if(fd == -1){
        perror("Eroare la deschidere fisier binar");
        exit(2);
    }

    element P;
    int bytes_read;
    while((bytes_read = read(fd, &P, sizeof(element))) > 0) {
        printf("%s %d\n", P.masca, P.count);
    }
    if(bytes_read == -1) {
        perror("Eroare la citire");
        exit(3);
    }
    close(fd);
}

off_t get_file_size(int fd) {
    off_t current_pos = lseek(fd, 0, SEEK_CUR); //salvez curent position
    off_t file_size = lseek(fd, 0, SEEK_END); //merg la final
    lseek(fd, current_pos, SEEK_SET); // revin la positia intiala
    return file_size; 
}

int read_element(int fd, off_t offset, element *p) {
    if (lseek (fd, offset, SEEK_SET) == -1) {
        perror("Eroare la pozitionarea fisierului");
        return -1;
    }
    return read(fd, p, sizeof(element));
}

int update_element(int fd, off_t offset, element *p) {
    if (lseek (fd, offset, SEEK_SET) == -1) {
        perror("Eroare la pozitionarea fisierului");
        return -1;
    }
    return write(fd, p, sizeof(element));
} 

void read_strcut(const char* file, int count, char path[]) {
    int fd = open(file, O_WRONLY);
    if( fd == -1) {
        perror("Eroare la deschiderea fisierului text:");
        return;
    }


    int intrari = get_file_size(fd) / sizeof(element);
    element e;

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = intrari * sizeof(element);
    lock.l_len = sizeof(element);

    if(fcntl(fd, F_SETLKW, &lock) == -1){
        perror("Eroare la lockarea fisierului:");
        return;
    }
    strcpy(e.masca, path);
    e.count = count;
    if(!update_element(fd, (intrari) * sizeof(element), &e)) {
        perror("Eroare la scrierea in fisier:");
        return;
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    if(close(fd) == -1) {
        perror("Eroare la inchiderea fisierului");
        exit(6);
    }
    return;
}

void parcurgere_recursiva(const char *cale, char* file, int count) {
    DIR *dir = opendir(cale);
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    char cale_completa[1024];
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(cale_completa, sizeof(cale_completa), "%s/%s", cale, entry->d_name);

        struct stat info;
        if (stat(cale_completa, &info) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            //printf("%s %d\n", cale_completa, count);
            read_strcut(file, count, cale_completa);
            parcurgere_recursiva(cale_completa, file, 0);
        } else {
            count += info.st_blocks;

        }

    }
    closedir(dir);
    
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("Numar insuficient de argumente.\n");
        return 1;
    }

    if(strcmp(argv[1], "-p") == 0) {
        print_file(argv[2]);
        return 0;
    }
    //printf("ARGV1: %s, \nARGV2: %s\n", argv[1], argv[2]);

    parcurgere_recursiva(argv[2], argv[1], 0);

    return 0;

}