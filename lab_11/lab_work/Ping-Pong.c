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


#define handle_err(err) \
    { perror(err); exit(EXIT_FAILURE); }

int fd_bin;

int read_flag(int fd) {
    int flag;
    lseek(fd, 0, SEEK_SET);
    read(fd, &flag, sizeof(int));
    return flag;
}

void write_flag(int fd, int flag) {
    lseek(fd, 0, SEEK_SET);
    if (write(fd, &flag, sizeof(int)) != sizeof(int)) {
        handle_err("Eroare la scrierea flagului");
    }
    fsync(fd);
}

int read_reply(int fd, char* reply) {
    char ch;
    int size = 0, bytes_read = 0;

    while (1) {
        if ((bytes_read = read(fd, &ch, sizeof(char))) < 0) {
            handle_err("Error at reading");
        }

        if (bytes_read == 0) {
            if (size == 0)
                return 2;
            else
                break;
        }

        reply[size++] = ch;
        if (ch == '\n') break;
    }

    reply[size] = '\0';
    return 1;
}


void dialog_tata() {
	int fd, flag = -1, status = -1;
    char reply[1024];
    if((fd = open("./replici-tata.txt", O_RDONLY)) < 0) {
        handle_err("Eroare la deschiderea flag.bin");
    }

    while(status != 2) {
        while(flag != 0 && flag != 2) {
            flag = read_flag(fd_bin);
        }
        status = read_reply(fd, reply);
        flag = -1;
        if(status == 1) printf("%s\n", reply);

        write_flag(fd_bin, 1);
    }
    write_flag(fd_bin, 2);
}

void dialog_fiu()
{
    int fd, flag = -1, status = -1;
    char reply[1024];
    if((fd = open("./replici-fiu.txt", O_RDONLY)) < 0) {
        handle_err("Eroare la deschiderea flag.bin");
    }

    while(status != 2) {
        while(flag != 1 && flag != 2) {
            flag = read_flag(fd_bin);
        }
        
        status = read_reply(fd, reply);
        flag = -1;
        if(status == 1) printf("%s\n", reply);

        write_flag(fd_bin, 0);
    }
    write_flag(fd_bin, 2);
}

int main()
{
	pid_t pid_fiu;

	// TODO: de inițializat cu 0 fișierul flag.bin
    int fd, a = 0;
    if((fd = open("./flag.bin", O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0) {
        handle_err("Eroare la deschiderea flag.bin");
    }

    write(fd, &a, sizeof(int));
    close(fd);

    if((fd_bin = open("./flag.bin", O_RDWR | O_SYNC)) < 0) {
        handle_err("Eroare la deschiderea flag.bin");
    }

	if(-1 == (pid_fiu=fork()) )
	{
		perror("Eroare la fork");  return 1;
	}

	/* Ramificarea execuției în cele două procese, tată și fiu. */
	if(pid_fiu == 0)
	{   /* Zona de cod executată doar de către fiu. */

		printf("\n[P1] Procesul fiu, cu PID-ul: %d.\n", getpid()); usleep(200);

		dialog_fiu();
	}
	else
	{   /* Zona de cod executată doar de către părinte. */

		printf("\n[P0] Procesul tata, cu PID-ul: %d.\n", getpid()); usleep(200);

		dialog_tata();
	}

	/* Zona de cod comună, executată de către ambele procese */
	printf("\nSfarsitul executiei procesului %s.\n\n", pid_fiu == 0 ? "fiu" : "parinte" );
	return 0;
}