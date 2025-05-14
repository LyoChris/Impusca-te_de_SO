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


typedef struct {
    int a;
    int b;
} element;

int read_ele(int fd, off_t offset, element *e) {
    if (lseek (fd, offset, SEEK_SET) == -1) {
        perror("Eroare la pozitionarea cursorului");
        return -1;
    }
    return read(fd, e, sizeof(element));
}

int write_ele(int fd, off_t offset, element *e) {
    if (lseek (fd, offset, SEEK_SET) == -1) {
        perror("Eroare la pozitionarea cursorului");
        return -1;
    }
    return write(fd, e, sizeof(element));
}

int N=0;

void calcul_tata() {
	int fd1, fd2;
    if((fd1 = open("./operanzi.bin", O_RDONLY)) == -1) {
        handle_err("Eroare la deschidere operanzi.bin");
    }

    if((fd2 = open("./operatori.txt", O_RDONLY)) == -1) {
        handle_err("Eroare la deschidere operatori.txt");
    }

    element e;
    char operator;
    for(int i = 0; i < N; i++) {
        int rezultat = 0;
        read_ele(fd1, i * sizeof(element), &e);
        read(fd2, &operator, sizeof(char));
        if(operator == '+') rezultat = e.a + e.b;
        if(operator == '-') rezultat = e.a - e.b;
        if(operator == '*') rezultat = e.a * e.b;
        if(operator == '/') rezultat = e.a / e.b;

        printf("%d %c %d = %d\n", e.a, operator, e.b, rezultat);
    }

    close(fd1);
    close(fd2);

}

void generare_fiu1() {
    int fd;
    if((fd = open("./operanzi.bin", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) {
        handle_err("Eroare la deschidere operanzi.bin");
    }

	srand(getpid());
    for(int i = 0; i < N; i++) {
        element e;
        e.a = rand() % 10000;
        e.b = rand() % 10000;
        write_ele(fd, i * sizeof(element), &e);
    }

    close(fd);
}

void generare_fiu2()
{
	int fd;
    char s[4] = "+-*/";
    if((fd = open("./operatori.txt", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) {
        handle_err("Eroare la deschidere operatori.txt");
    }

    srand(getpid());
    for(int i = 0; i < N; i++) {
        int a = rand() % 4;
        write(fd, &s[a], sizeof(char));
    }

    close(fd);
}

int main()
{
	pid_t pid_fiu1, pid_fiu2;

	printf("Dati numarul intreg N:");
	scanf("%d", &N); // TODO: tratați excepțiile de citire a unui număr întreg


	/* Crearea procesului fiu #1. */
	if(-1 == (pid_fiu1=fork()) )
	{
		perror("Eroare la fork #1");  return 1;
	}

	/* Ramificarea execuției după primul apel fork. */
	if(pid_fiu1 == 0)
	{   /* Zona de cod executată doar de către fiul #1. */
		printf("\n[P1] Procesul fiu 1, cu PID-ul: %d.\n", getpid());

		generare_fiu1();
		return 0;
	}
	else
	{   /* Zona de cod executată doar de către părinte. */
		printf("\n[P0] Procesul tata, cu PID-ul: %d.\n", getpid());

		/* Crearea procesului fiu #2. */
		if(-1 == (pid_fiu2=fork()) )
		{
			perror("Eroare la fork #2");  return 2;
		}
		
		/* Ramificarea execuției după al doilea apel fork. */
		if(pid_fiu2 == 0)
		{   /* Zona de cod executată doar de către fiul #2. */
			printf("\n[P2] Procesul fiu 2, cu PID-ul: %d.\n\n", getpid());

			generare_fiu2();
			return 0;
		}
		else
		{   /* Zona de cod executată doar de către părinte. */

			wait(NULL);
			wait(NULL);
			// Mai întâi aștept terminarea ambilor fii, iar apoi fac calculul.

			calcul_tata();
			return 0;
		}
	}
}