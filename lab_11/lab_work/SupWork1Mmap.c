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

int N=0;
int* map;

void calcul_tata() {
    for(int i = 0; i < 2 * N; i+=2) {
        int rezultat;
        switch((char)(map[2 * N + ((i + 1)/2)])) {
            case '+' :
                rezultat = map[i] + map[i + 1];
                break;
            case '-' :
                rezultat = map[i] - map[i + 1];
                break;
            case '*' :
                rezultat = map[i] * map[i + 1];
                break;
            case '/' :
                rezultat = map[i] / map[i + 1];
                break;
        }
        printf("%d %c %d = %d \n", map[i], map[2 * N + ((i + 1)/2)], map[i + 1], rezultat);
    }
}

void generare_fiu1() {
    srand(getpid());
    for(int i = 0; i < 2 * N; i++) {
        map[i] = rand() % 10000;
    }
}

void generare_fiu2()
{
    char s[4] = "+-*/";
    srand(getpid());
    for(int i = 2 * N; i < 3 * N; i++) {
        map[i] = (int)(s[rand() % 4]);
    }
}

int main()
{
	pid_t pid_fiu1, pid_fiu2;

	printf("Dati numarul intreg N:");
	scanf("%d", &N); // TODO: tratați excepțiile de citire a unui număr întreg

    int map_size = (N * sizeof(int) * 3);
    map = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (map == MAP_FAILED) handle_err("Error at mmap");

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