#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>    // necesar pentru functia getpwuid
#include <grp.h>    // necesar pentru functia getgrgid
#include <limits.h> // necesar pentru PATH_MAX

typedef struct {
    float id;
    float cant;
} Produs;

int lock_file_seg(int fd, int index) {
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = index * sizeof(Produs);
    lock.l_len = sizeof(Produs);

    return fcntl(fd, F_SETLKW, &lock);

    /*
    while (fcntl(fd, F_SETLKw, &lock) == -1 && (errno == EAGAIN || errno == EACCES)) {
        if (errno == EAGAIN || errno == EACCES) usleep(RETRY_INTERVAL);
        else {
            perror(Eroare fatala la lock de fisiere);
            return -1;
        }

    }

    return 0;*/
}

int unlock_file_seg(int fd, int index) {
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = index * sizeof(Produs);
    lock.l_len = sizeof(Produs);
    return fcntl(fd, F_SETLK, &lock);
}

off_t get_file_size(int fd) {
    off_t current_pos = lseek(fd, 0, SEEK_CUR); //salvez curent position
    off_t file_size = lseek(fd, 0, SEEK_END); //merg la final
    printf("MAMA %ld", current_pos);
    lseek(fd, current_pos, SEEK_SET); // revin la positia intiala
    return file_size; 
}

int read_product(int fd, off_t offset, Produs *p) {
    if (lseek (fd, offset, SEEK_SET) == -1) {
        perror("Eroare la pozitionarea fisierului");
        return -1;
    }
    return read(fd, p, sizeof(Produs));
}

int update_product(int fd, off_t offset, Produs *p) {
    if (lseek (fd, offset, SEEK_SET) == -1) {
        perror("Eroare la pozitionarea fisierului");
        return -1;
    }
    return write(fd, p, sizeof(Produs));
}

void process_instruction(const char* instruc, const char* db_filename) {
    int fd = open(db_filename, O_RDWR);
    if (fd == -1) {
        perror("Eroare la deschiderea fisierului");
        return;
    }
    
    float cod_prod;
    float cantitate;
    char operatiune;

    if (sscanf(instruc, "%f %c%f", &cod_prod, &operatiune, &cantitate) == -1) {
        perror("Eroare la citirea instrctiunilor");
        close(fd);
        return;
    }
    printf("%f, %c, %f\n", cod_prod, operatiune, cantitate);

    off_t file_size = get_file_size(fd);
    int nume_records = file_size / sizeof(Produs);
    int found = 0;
    Produs p;
    printf("%d\n", nume_records);

    for (int i = 0; i < nume_records; i++) {
        off_t offset = i * sizeof(Produs);
        printf("%d         ,%ld         ", i, offset);
        if (read_product(fd, offset, &p) && p.id == cod_prod) {
            found = i;
            printf("%f, %f\n", p.id, p.cant);
            if(lock_file_seg(fd, offset) == -1) {
                close(fd);
                return;
            }

            if(operatiune == '+') {
                p.cant +=cantitate;
            }
            else {
                if(operatiune == '-' ) {
                    if(p.cant >= cantitate) p.cant -= cantitate;
                    else {
                        fprintf(stderr, "Stoc insuficent pentru produsul %f\n", cod_prod);
                        exit(2);
                    }
                }
            }

            if (update_product(fd, offset, &p) == -1) {
                unlock_file_seg(fd, offset);
                close(fd);
                return;
            }

            unlock_file_seg(fd, offset);
            break;
        }
    }

    if (!found) {
        if (operatiune == '+') {
            p.id = cod_prod;
            p.cant = cantitate;
            off_t offset = nume_records * sizeof(Produs);
            if (update_product(fd, offset, &p) == -1) {
                close(fd);
                return;
            }
        } else {
            fprintf(stderr, "Produsul cu codul %f nu exista\n", cod_prod);
            return;
        }
    }

    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        write(2, "Utilizare: <program> <fisier_instructiuni>\n", 41);
        return 1;
    }

    int instr_file = open(argv[1], O_RDONLY);
    if (instr_file == -1) {
        perror("Eroare la deschiderea fișierului de instrucțiuni");
        return 2;
    }

    char instruction[256];
    ssize_t bytes_read;
    while ((bytes_read = read(instr_file, instruction, sizeof(instruction) - 1)) > 0) {
        instruction[bytes_read] = '\0';
        printf("%s\n", instruction);
        process_instruction(instruction, "/home/lyo/Desktop/Facultate/SO/lab_7/stocuri.bin");
        usleep(500000);
    }

    if (bytes_read == -1) {
        perror("Eroare la citirea fișierului de instrucțiuni");
    }

    close(instr_file);
    return 0;
}