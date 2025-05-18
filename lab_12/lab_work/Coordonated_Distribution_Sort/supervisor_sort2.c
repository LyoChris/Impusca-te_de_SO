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
#include <semaphore.h>
#include <limits.h>

void distro_lines(char* map, int size, char** shm_maps, int n, int* volumes) {
    int *offset = calloc(n, sizeof(*offset));
    int cur = 0;
    char *segs_line = map;

    for(int i = 0; i < size; i++) {
        if(map[i] == '\n' || i+1 == size) {
            int len = &map[i] - segs_line + 1;
            memcpy(shm_maps[cur] + offset[cur], segs_line, len);
            offset[cur] += len;
            volumes[cur] += len;
            cur = (cur + 1) % n;
            segs_line = map + i + 1;
        }
    }
}

void interclasare_segs(char **shm_maps, int *volumes, int n) {
    char **cur = malloc(n * sizeof *cur);
    char **end = malloc(n * sizeof *end);
    if (!cur || !end) { perror("malloc"); exit(EXIT_FAILURE); }
    for (int i = 0; i < n; i++) {
        cur[i] = shm_maps[i];
        end[i] = shm_maps[i] + volumes[i];
    }
    while (1) {
        int best = -1;
        for (int i = 0; i < n; i++) {
            if (cur[i] < end[i]) { best = i; break; }
        }
        if (best < 0) break;
        for (int i = best + 1; i < n; i++) {
            if (cur[i] < end[i]) {
                char *a = cur[i];
                char *b = cur[best];
                char *pa, *pb; 
                if (!(pa = strchr(a, ':'))) 
                    continue;

                if (!(pa = strchr(pa + 1, ':'))) 
                    continue;
                int ka = atoi(pa + 1);
                if(!(pb = strchr(b, ':'))) 
                    continue;
                if(!(pb = strchr(pb + 1, ':'))) 
                    continue;
                int kb = atoi(pb + 1);
                if (ka < kb || (ka == kb && strcmp(a, b) < 0)) {
                    best = i;
                }
            }
        }
        char *nl = strchr(cur[best], '\n');
        size_t len = nl ? (nl - cur[best] + 1)
                        : (end[best] - cur[best]);
        write(STDOUT_FILENO, cur[best], len);
        cur[best] += len;
    }
    free(cur);
    free(end);
}

int number_of_lines(char* map, int size) {
    int nr = 0;
    for(int i = 0; i < size; i++) {
        if(map[i] == '\n') nr++;
    }
    return nr;
}

int main(int argc, char* argv[]) {
    int n, fd;
    char* filename, * map;
    struct stat st;
    if( argc < 3 || argc > 3) {
        fprintf(stderr, "Usage: ./supervisor_sort2 [number of workers] [file to be sorted]\n");
        exit(1);
    }
    else {
        for(int i = 0; i < strlen(argv[1]); i++) {
            if(!isdigit(argv[1][i])){
                fprintf(stderr, "Second argument must be a natural number > 0\n");
                exit(2);
            }
        }
        n = atoi(argv[1]);
        filename = malloc(strlen(argv[2]) + 1);
        strcpy(filename, argv[2]);
    }

    if((fd = open(filename, O_RDONLY)) == -1) {
        perror("Eroare la deschidere fisier");
        exit(3);
    }

    if(fstat(fd, &st) < 0) {
        perror("Eroare la fstat");
        exit(4);
    }

    map = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) { 
        perror("mmap"); 
        exit(1); 
    }
    
    char **shm_maps = malloc(n * sizeof(char*));
    int *volumes = calloc(n, sizeof(int));
    int *shm_fds = malloc(n * sizeof(int));
    char (*shm_names)[64] = malloc(n * 64);
    int shm_size = 4096;

    for(int i = 0; i < n; i++) {
        char name[64];
        snprintf(name, sizeof(shm_names[i]), "%s_%d_%d", "/shm_worker", i, getpid());
        strcpy(shm_names[i], name);
        int sfd;

        if((sfd = shm_open(shm_names[i], O_CREAT | O_RDWR, 0600)) < 0) {
            perror("Eroare la shm_open");
            exit(3);
        }

        if(ftruncate(sfd, shm_size) < 0) {
            perror("Eroare la ftruncate");
            exit(7);
        }

        void *addr = mmap(NULL, shm_size, PROT_READ |  PROT_WRITE, MAP_SHARED, sfd, 0);
        if (addr == MAP_FAILED) { 
            perror("mmap shm"); 
            exit(8); 
        }
        //shm_unlink(shm_names[i]);

        shm_fds[i] = sfd;
        shm_maps[i] = addr;
        volumes[i] = 0;
    }

    distro_lines(map, st.st_size, shm_maps, n, volumes);    
    if(munmap(map, st.st_size) < 0) {
        perror("Eroare la unmap");
        exit(2);
    }

    for(int i = 0; i < n; i++) {
        pid_t pid;
        if((pid = fork()) == -1) {
            perror("Eroare la fork");
            exit(9);
        }

        if(pid == 0) {
            char volume[32];
            snprintf(volume, sizeof(volume), "%d", volumes[i]);
            execlp("./worker_sort2", "worker_sort2", shm_names[i], volume, NULL);

            perror("Eroare la apelul execlp");
            return 100;
        }

    }
    int error = 0;
    for(int i = 0; i < n; i++) {
        int status;
        pid_t p = wait(&status);
        if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
            error = 1;
            fprintf(stderr, "[Supervisor] Worker %d failed (code=%d)\n", (int)p, WIFEXITED(status) ? WEXITSTATUS(status) : -WTERMSIG(status));
        }
    }

    if(!error) {
        interclasare_segs(shm_maps, volumes, n);
    }
    else {
        fprintf(stderr, "[Supervisor] Abandon due to worker errors\n");
    }

    for (int i = 0; i < n; i++) {
        munmap(shm_maps[i], shm_size);
        close(shm_fds[i]);
    }

    free(shm_maps);
    free(volumes);
    free(shm_fds);
    free(shm_names);

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}