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
#include <limits.h>

typedef struct optiune {
    int i, u;
    int index;
} Optiuni;

Optiuni procesare_opt(int argc, char* argv[]) {
    Optiuni opt = {0, 0, 0};
    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-i") == 0){
            opt.i = 1;
            opt.index++;
        }
        if(strcmp(argv[i], "-u") == 0){
            opt.u = 1;
            opt.index++;
        }
    }

    return opt;
}

void my_cp_to_file(char* source, char* dest, Optiuni opt) {
    struct stat st_source, st_dest;
    int fd_source, fd_dest;

    if (stat(source, &st_source) == -1) {
        perror("stat sursă");
        return;
    }

    bool should_copy = true;
    if ( stat(dest, &st_dest) != -1) {
        if(opt.u && st_source.st_mtime <= st_dest.st_mtime) should_copy = false;
        if(opt.i && should_copy) {
            printf("Fisierul '%s' deja exista. Supracrii? (y/n): ", dest);
            char answer;
            while(answer != 'y' || answer != 'Y' || answer != 'N' || answer != 'n') {
                scanf(" %c", &answer);
            }
            if (answer != 'y' && answer != 'Y') {
                should_copy = false;
            }
        }
    }

    if(!should_copy) return;

    if ((fd_source = open(source, O_RDONLY)) < 0) {
        perror("open sursă");
        return;
    }

    if ((fd_dest = open(dest, O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
        perror("open dest");
        close(fd_source);
        return;
    }

    if (ftruncate(fd_dest, st_source.st_size) == -1) {
        perror("ftruncate");
        close(fd_source);
        close(fd_dest);
        return;
    }

    void* src_map = mmap(NULL, st_source.st_size, PROT_READ, MAP_PRIVATE, fd_source, 0);
    if (src_map == MAP_FAILED) {
        perror("mmap sursă");
        close(fd_source);
        close(fd_dest);
        return;
    }

    void* dest_map = mmap(NULL, st_source.st_size, PROT_WRITE, MAP_SHARED, fd_dest, 0);
    if (dest_map == MAP_FAILED) {
        perror("mmap destinație");
        munmap(src_map, st_source.st_size);
        close(fd_source);
        close(fd_dest);
        return;
    }

    memcpy(dest_map, src_map, st_source.st_size);

    munmap(src_map, st_source.st_size);
    munmap(dest_map, st_source.st_size);
    close(fd_source);
    close(fd_dest);
}

void my_cp(char* source, char* dest, Optiuni opt) {
    struct stat st;
    if (stat(dest, &st) == -1 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "%s nu este un director valid\n", dest);
        return;
    }

    struct stat st_source;
    if (stat(source, &st_source) == -1) {
        perror("stat sursă");
        return;
    }

    if (S_ISDIR(st_source.st_mode)) {
        fprintf(stderr, "Omit %s (este director)\n", source);
        return;
    }

    char* base = strrchr(source, '/');
    base = base ? base + 1 : source;

    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", dest, base);



    my_cp_to_file(source, full_path, opt);

}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [-i] [-u] sursa... destinatie\n", argv[0]);
        return 1;
    }

    Optiuni opt = procesare_opt(argc, argv);
    char* dest = argv[argc - 1];
    struct stat st_dest;
    bool dest_exists = (stat(dest, &st_dest) == 0);
    bool dest_is_dir = dest_exists && S_ISDIR(st_dest.st_mode);
    int nr_sur = argc - 1 - opt.index;

    if (nr_sur > 3 && (!dest_exists || !dest_is_dir)) {
        fprintf(stderr, "%s: NU pot copia multiple fișiere într-un fișier inexistent sau non-director.\n", argv[0]);
        return 2;
    }

    for (int i = opt.index + 1; i < argc - 1; i++) {
        if (nr_sur < 3) {
            my_cp_to_file(argv[i], dest, opt);
            return 0;
        }
        my_cp(argv[i], dest, opt);
    }

    return 0;
}