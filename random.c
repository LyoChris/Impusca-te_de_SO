#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <math.h>
#include <errno.h>

#define FIFO_NAME "fifo_file"

void send_data_to_worker1(int pipefd[2], FILE *file) {
    float a, b, c;
    while (fscanf(file, "%f %f %f", &a, &b, &c) == 3) {
        write(pipefd[1], &a, sizeof(float));
        write(pipefd[1], &b, sizeof(float));
        write(pipefd[1], &c, sizeof(float));
    }
}

void process_received_data(int fifo_fd) {
    float a, b, c, P, S;
    int count_valid = 0, count_invalid = 0;

    while (read(fifo_fd, &a, sizeof(float)) > 0) {
        read(fifo_fd, &b, sizeof(float));
        read(fifo_fd, &c, sizeof(float));
        read(fifo_fd, &P, sizeof(float));
        read(fifo_fd, &S, sizeof(float));

        if (P != 0) {
            printf("Tripletul %f, %f, %f formează un triunghi cu perimetrul %f și aria %f\n", a, b, c, P, S);
            count_valid++;
        } else {
            printf("Tripletul %f, %f, %f nu poate forma un triunghi\n", a, b, c);
            count_invalid++;
        }
    }

    printf("Triplete valide: %d\n", count_valid);
    printf("Triplete invalide: %d\n", count_invalid);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizare: %s <input_data.txt>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Verifică existența fișierului
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Eroare la deschiderea fișierului");
        exit(EXIT_FAILURE);
    }

    // Creează FIFO pentru comunicarea cu worker3
    if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST) {
        perror("Eroare la crearea FIFO");
        exit(EXIT_FAILURE);
    }

    int fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd == -1) {
        perror("Eroare la deschiderea FIFO");
        exit(EXIT_FAILURE);
    }

    // Creează canalul anonim pentru comunicarea cu worker1
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Eroare la crearea pipe-ului");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Eroare la fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execl("./worker1", "worker1", (char *) NULL);
        perror("Eroare la exec");
        exit(EXIT_FAILURE);
    }

    // Trimite datele către worker1
    send_data_to_worker1(pipefd, file);
    close(pipefd[1]);

    // Procesează datele primite de la worker3
    process_received_data(fifo_fd);

    fclose(file);
    close(fifo_fd);

    return 0;
}
2. worker1.c
c
Copy
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define SHM_NAME "/shared_memory"

void process_data_and_send_to_worker2(int pipefd[2], int shm_fd) {
    float a, b, c, r;
    while (read(pipefd[0], &a, sizeof(float)) > 0) {
        read(pipefd[0], &b, sizeof(float));
        read(pipefd[0], &c, sizeof(float));

        // Verifică dacă tripletul poate forma un triunghi
        if (a > 0 && b > 0 && c > 0 && a + b > c && a + c > b && b + c > a) {
            r = 1;  // Validează triunghiul
        } else {
            r = 0;  // Nu este un triunghi valid
        }

        // Creează și deschide memoria partajată
        void *shm_ptr = mmap(NULL, sizeof(float) * 4, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shm_ptr == MAP_FAILED) {
            perror("Eroare la mmap");
            exit(EXIT_FAILURE);
        }

        // Scrie rezultatele în memoria partajată
        memcpy(shm_ptr, &a, sizeof(float));
        memcpy(shm_ptr + sizeof(float), &b, sizeof(float));
        memcpy(shm_ptr + 2 * sizeof(float), &c, sizeof(float));
        memcpy(shm_ptr + 3 * sizeof(float), &r, sizeof(float));

        // Mapează rezultatele către worker2
        msync(shm_ptr, sizeof(float) * 4, MS_SYNC);
    }
}

int main() {
    // Creează și deschide memoria partajată
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Eroare la creare memorie partajată");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, sizeof(float) * 4);

    // Creează canalul anonim pentru a primi date de la supervisor
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Eroare la crearea pipe-ului");
        exit(EXIT_FAILURE);
    }

    // Procesează datele și trimite rezultatele către worker2
    process_data_and_send_to_worker2(pipefd, shm_fd);

    close(pipefd[0]);
    close(shm_fd);

    return 0;
}
3. worker2+3.c
c
Copy
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#define SHM_NAME "/shared_memory"
#define SHM_NAME_ANON "/shared_memory_anon"

void worker2_function(int shm_fd, int shm_anon_fd) {
    // Mapare memorie partajată pentru a citi date de la worker1
    void *shm_ptr = mmap(NULL, sizeof(float) * 4, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Eroare la mmap");
        exit(EXIT_FAILURE);
    }

    // Mapare memorie partajată anonimă pentru a trimite date la worker3
    void *shm_anon_ptr = mmap(NULL, sizeof(float) * 4, PROT_READ | PROT_WRITE, MAP_SHARED, shm_anon_fd, 0);
    if (shm_anon_ptr == MAP_FAILED) {
        perror("Eroare la mmap anonim");
        exit(EXIT_FAILURE);
    }

    float a, b, c, r, p;
    while (1) {
        // Citește datele de la worker1
        memcpy(&a, shm_ptr, sizeof(float));
        memcpy(&b, shm_ptr + sizeof(float), sizeof(float));
        memcpy(&c, shm_ptr + 2 * sizeof(float), sizeof(float));
        memcpy(&r, shm_ptr + 3 * sizeof(float), sizeof(float));

        if (r == 1) {
            p = (a + b + c) / 2.0;  // Semi-perimetrul
        } else {
            p = 0;
        }

        // Scrie rezultatele către worker3
        memcpy(shm_anon_ptr, &a, sizeof(float));
        memcpy(shm_anon_ptr + sizeof(float), &b, sizeof(float));
        memcpy(shm_anon_ptr + 2 * sizeof(float), &c, sizeof(float));
        memcpy(shm_anon_ptr + 3 * sizeof(float), &p, sizeof(float));
        msync(shm_anon_ptr, sizeof(float) * 4, MS_SYNC);
    }
}

void worker3_function(int shm_anon_fd) {
    // Mapare memorie partajată anonimă
    void *shm_anon_ptr = mmap(NULL, sizeof(float) * 4, PROT_READ | PROT_WRITE, MAP_SHARED, shm_anon_fd, 0);
    if (shm_anon_ptr == MAP_FAILED) {
        perror("Eroare la mmap anonim");
        exit(EXIT_FAILURE);
    }

    float a, b, c, p, P, S;
    while (1) {
        // Citește datele de la worker2
        memcpy(&a, shm_anon_ptr, sizeof(float));
        memcpy(&b, shm_anon_ptr + sizeof(float), sizeof(float));
        memcpy(&c, shm_anon_ptr + 2 * sizeof(float), sizeof(float));
        memcpy(&p, shm_anon_ptr + 3 * sizeof(float), sizeof(float));

        if (p != 0) {
            P = 2 * p;  // Perimetrul
            S = sqrt(p * (p - a) * (p - b) * (p - c));  // Aria
        } else {
            P = 0;
            S = 0;
        }

        // Trimiterea rezultatelor către supervisor
        int fifo_fd = open("fifo_file", O_WRONLY);
        if (fifo_fd == -1) {
            perror("Eroare la deschiderea FIFO");
            exit(EXIT_FAILURE);
        }
        write(fifo_fd, &a, sizeof(float));
        write(fifo_fd, &b, sizeof(float));
        write(fifo_fd, &c, sizeof(float));
        write(fifo_fd, &P, sizeof(float));
        write(fifo_fd, &S, sizeof(float));
        close(fifo_fd);
    }
}

int main() {
    // Creează memoria partajată anonimă
    int shm_anon_fd = shm_open(SHM_NAME_ANON, O_CREAT | O_RDWR, 0666);
    if (shm_anon_fd == -1) {
        perror("Eroare la creare memorie partajată anonimă");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_anon_fd, sizeof(float) * 4);

    pid_t pid = fork();
    if (pid == -1) {
        perror("Eroare la fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Funcția worker2
        worker2_function(0, shm_anon_fd);
        exit(EXIT_SUCCESS);
    }

    // Funcția worker3
    worker3_function(shm_anon_fd);

    return 0;
}