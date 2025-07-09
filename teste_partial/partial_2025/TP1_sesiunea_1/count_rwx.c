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
    unsigned short masca;
    unsigned int count;
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
        printf("%hu %u\n", P.masca, P.count);
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

void read_strcut(const char* file, unsigned short value) {
    int fd = open(file, O_RDWR);
    if( fd == -1) {
        perror("Eroare la deschiderea fisierului text:");
        return;
    }


    int intrari = get_file_size(fd) / sizeof(element);
    int gasit = 0;
    element e;

    for(int i = 0; i<intrari; i++) {
        int index = i * sizeof(element);
        if(read_element(fd, index, &e) && e.masca == value) {
            gasit = i;
            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = i * sizeof(element);
            lock.l_len = sizeof(element);

            if(fcntl(fd, F_SETLKW, &lock) == -1){
                perror("Eroare la lockarea fisierului:");
                exit(3);
            }

            e.count++;

            if(!update_element(fd, index, &e)) {
                perror("Eroare la scrierea in fisier");
                exit(4);
            }

            lock.l_type = F_UNLCK;
            if(fcntl(fd, F_SETLK, &lock) == -1){
                perror("Eroare la deblocarea fisierului:");
                exit(5);
            }
            if(close(fd) == -1) {
                perror("Eroare la inchiderea fisierului");
                exit(6);
            }
            return;
        }
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = intrari * sizeof(element);
    lock.l_len = sizeof(element);

    if(fcntl(fd, F_SETLKW, &lock) == -1){
        perror("Eroare la lockarea fisierului:");
        return;
    }

    if(gasit == 0) {
        e.masca = value;
        e.count = 1;
        if(!update_element(fd, (intrari) * sizeof(element), &e)) {
            perror("Eroare la scrierea in fisier:");
            return;
        }
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &lock);
    }
    if(close(fd) == -1) {
        perror("Eroare la inchiderea fisierului");
        exit(6);
    }
    return;
}

void parcurgere_recursiva(const char *cale, char* file) {
    DIR *dir = opendir(cale);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char cale_completa[1024];
        snprintf(cale_completa, sizeof(cale_completa), "%s/%s", cale, entry->d_name);

        struct stat info;
        if (stat(cale_completa, &info) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            unsigned short perm_mask = info.st_mode & 0777;
            read_strcut(file, perm_mask);
            parcurgere_recursiva(cale_completa, file);
        } else {
            unsigned short perm_mask = info.st_mode & 0777;
            read_strcut(file, perm_mask);
        }

    }

    closedir(dir);
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("Numar insuficient de argumente.");
        return 1;
    }

    if(strcmp(argv[1], "-o") == 0) {
        print_file(argv[2]);
        return 0;
    }

    parcurgere_recursiva(argv[2], argv[1]);

    return 0;

}