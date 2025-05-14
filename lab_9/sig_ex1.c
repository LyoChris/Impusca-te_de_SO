#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

#define NR_AFISARI 100

void handle_sigusr1(int nr_sem) {
    fprintf(stderr, "Proceesul %d a primit semnlul de not.\n", getpid());
}

int main() {
    int pid;
    sigset_t masca_usr1_blocat, masca_usr1_deblocat;

    sigemptyset(&masca_usr1_deblocat);
    sigdelset(&masca_usr1_blocat, SIGUSR1);

    if(SIG_ERR == signal(SIGUSR1, handle_sigusr1)) {
        perror("Eroare la semnal");
        exit(1);
    }

    if((pid = fork()) == -1) {
        perror("Eroare la fork");
        exit(3);
    }

    if(pid == 0) {
        /*SON*/
        for(int i = 0; i < NR_AFISARI; i++) {
             printf("hi-");
             fflush(stdout);
             kill(getppid(), SIGUSR1);
             sigsuspend(&masca_usr1_deblocat);
        }
    }
    else {
        /*FATHER*/
        for(int i = 0; i < NR_AFISARI; i++) {
            sigsuspend(&masca_usr1_deblocat);
			printf("ho, ");
            fflush(stdout);
			kill(pid,SIGUSR1);
        }
        printf("\n final \n");
    }

    return 0;
}