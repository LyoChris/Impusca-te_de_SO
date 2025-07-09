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
    char c;
    unsigned int count;
} camp;

off_t get_file_size(int fd) {
    off_t current_pos = lseek(fd, 0, SEEK_CUR); //salvez curent position
    off_t file_size = lseek(fd, 0, SEEK_END); //merg la final
    lseek(fd, current_pos, SEEK_SET); // revin la positia intiala
    return file_size; 
}

int read_camp(int fd, off_t offset, camp *e) {
    if (lseek (fd, offset, SEEK_SET) == -1) {
        perror("Eroare la pozitionarea cursorului");
        return -1;
    }
    return read(fd, e, sizeof(camp));
}

int write_camp(int fd, off_t offset, camp *e) {
    if (lseek (fd, offset, SEEK_SET) == -1) {
        perror("Eroare la pozitionarea cursorului");
        return -1;
    }
    return write(fd, e, sizeof(camp));
}

void printare(char* file) {
    int fd=open(file, O_RDONLY);
    if(fd == -1){
        perror("Eroare la deschidere fisier binar");
        exit(2);
    }

    camp e;
    int bytes_read;
    while((bytes_read = read(fd, &e, sizeof(camp))) == sizeof(camp)) {
        printf("%c %u\n", e.c, e.count);
    }
    if(bytes_read == -1) {
        perror("Eroare la citire");
        exit(3);
    }
    if(close(fd) == -1){
        perror("Eroare la inchiderea fisierului:");
        exit(2);
    }
}

void update_intrare(char*file, unsigned int v[]){
    int fd = open(file, O_WRONLY);
    if(fd == -1){
        perror("Eroare la deschiderea fisierului:");
        exit(2);
    }

    camp p;
    struct flock l;
    l.l_type = F_WRLCK;
    l.l_whence = SEEK_SET;

    int records = get_file_size(fd) / sizeof(camp);
    for(int i=0;i<=9;i++) {
        int offset = i * sizeof(camp);
        if(i >= records){
            l.l_start = offset;
            l.l_len = sizeof(camp);

            if(fcntl(fd, F_SETLKW, &l) == -1) {
                perror("Eroare la blocarea fisierului");
                exit(3);
            }

            read_camp(fd, offset, &p);
            p.c = i + '0';
            p.count = v[i];
            write_camp(fd, offset, &p);

            l.l_type = F_UNLCK;
            if(fcntl(fd, F_SETLKW, &l) == -1) {
                perror("Eroare la deblocarea fisierului");
                exit(3);
            }
        }
        else{
            l.l_start = offset;
            l.l_len = sizeof(camp);

            if(fcntl(fd, F_SETLKW, &l) == -1) {
                perror("Eroare la blocarea fisierului");
                exit(3);
            }

            read_camp(fd, offset, &p);
            p.count = p.count + v[i];
            write_camp(fd, offset, &p);

            l.l_type = F_UNLCK;
            if(fcntl(fd, F_SETLKW, &l) == -1) {
                perror("Eroare la deblocarea fisierului");
                exit(3);
            }

        }
    }
    if(close(fd) == -1){
        perror("Eroare la inchiderea fisierului:");
        exit(2);
    }
}

void parcurgere_dir(const char *cale, char* file, char word[]) {
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

        }
        else{
            if(strstr(entry->d_name, word)) {
                unsigned int v[10]={0};
                int fd = open(cale_completa, O_RDONLY);

                int bytes_read;
                char buffer[4096];

                while((bytes_read = read(fd, buffer, sizeof(buffer))) > 0 ){
                    for(int i=0;i<bytes_read; i++) {
                        for(int k=0;k<=9;k++) {
                            char k1 = k + '0';
                            if((char)k1 == buffer[i]) v[k]++;
                        }
                    }
                }
                if(bytes_read ==  -1){
                    printf("Eroare la citire\n");
                    exit(4);
                }

                update_intrare(file, v);
            }
        }
            //snprintf(cale_completa, sizeof(cale_completa), "%s/%s", cale, entry->d_name);

        

    }
    closedir(dir);
    
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("Utilizare incorecta\n");
        exit(2);
    }

    if(strcmp(argv[1], "--words") == 0) {
        printare(argv[2]);
        return 0;
    }
    int input_fd = open(argv[2], O_RDONLY);
    if(input_fd == -1) {
        perror("Eroare la deschiderea fisierului:");
        exit(1);
    }
    
    char director[4096];
    getcwd(director, sizeof(director));
    char instruction[256];
    int pos = 0;
    char ch;
    ssize_t bytes;
    
    while ((bytes = read(input_fd, &ch, 1)) == 1) {
        if (ch == '\n') {
            instruction[pos] = '\0';
            if (pos > 0) {
                parcurgere_dir(director, argv[1], instruction);
            }
            pos = 0;
        } else {
            if (pos < 255) {
                instruction[pos++] = ch;
            }
        }
    }

    if (pos > 0) {
        instruction[pos] = '\0';
        parcurgere_dir(director, argv[1], instruction);
    }

    close(input_fd);
    return 0;
}