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

int main(int argc, char* argv[]) {
    int op1, op2;
    char ch = NULL;
    while(1) {
        printf("MyCalculator> Dati doua numere: ");
        scanf(" %d", &op1);
        scanf(" %d", &op2);
        printf("MyCalculator> Dati operatorul: ");
        scanf(" %c", &ch);
        printf("%d %c %d\n", op1, ch, op2);
    }
}