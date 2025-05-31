#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_INACTIVE 5
#define BUFF_SIZE 1024

char* filename;
volatile int interval, no_input = 1, inactive_intervals = 0, timeout = 0, child_done = 0;
pid_t child_id = 0;

void disable_buffering() {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &newt);
    newt.c_lflag &= ~(ICANON);  // Dezactivează buffering-ul de linie
    newt.c_lflag |= ECHO;  // Păstrează afișarea caracterelor
    newt.c_cc[VMIN] = 1;  // Citirea unui singur caracter
    newt.c_cc[VTIME] = 0; // Fără timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}   

void enable_buffering() {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &newt);
    newt.c_lflag |= (ICANON | ECHO);  // Activează buffering-ul de linie și afișarea caracterelor
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void handle_alarm(int sig) {
    if(no_input) {
        inactive_intervals++;
        if(inactive_intervals < MAX_INACTIVE) {
            printf("\nAvertisment: %d secunde fara input...\n", interval * inactive_intervals);
            alarm(interval);
        } else {
            printf("TIMEOUT \n");
            timeout = 1;
        }
    } else {
        no_input = 1;
        inactive_intervals = 0;
        alarm(interval);
    }
}

void handle_sigusr2(int sig) {}

void handle_sigchld(int sig) {
    wait(NULL);
    child_done = 1;
}

void run_child () {
    if (signal(SIGUSR2, handle_sigusr2) == SIG_ERR) {
        perror("child: signal");
        exit(1);
    }
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR2);

    sigsuspend(&mask);

    printf("Fiul: am primit USR2!\n");
    //enable_buffering();
    int fd;
    if ((fd = open(filename, O_RDONLY)) < 0) {
        perror("Fiul: nu pot deschide fisierul");
        exit(1);
    }

    char buffer[BUFF_SIZE];
    ssize_t bytes;
    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes);
        //fprintf(stdout, "%s", buffer);
        //fflush(stdout);
    }
    close(fd);

    printf("Fiul: sfarsit executie!\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <nume_fisier> <interval> \n", argv[0]);
        exit(1);
    }

    filename = argv[1];
    interval = atoi(argv[2]);
    if (interval <= 0) {
        fprintf(stderr, "N trebuie sa fie > 0.\n");
        exit(1);
    }

    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) == -1) {
        perror("sigprocmask");
        exit(1);
    }

    if ((child_id = fork()) < 0) {
        perror("fork");
        exit(1);
    }

    if (child_id == 0) { 
        sigset_t child_mask;
        sigemptyset(&child_mask);
        if (sigprocmask(SIG_SETMASK, &child_mask, NULL) == -1) {
            perror("child: sigprocmask");
            exit(1);
        }

        run_child(); 
    }

    if (signal(SIGALRM, handle_alarm) == SIG_ERR) {
        perror("signal");
        kill(child_id, SIGKILL);
        exit(1);
    }

    if (signal(SIGCHLD, handle_sigchld) == SIG_ERR) {
        perror("signal");
        kill(child_id, SIGKILL);
        exit(1);
    }

    FILE* fout = fopen(filename, "w");
    if (!fout) {
        perror("Parinte: fopen");
        kill(child_id, SIGKILL);
        exit(1);
    }
    /*int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("parinte: open");
        kill(child_id, SIGKILL);
        exit(1);
    }*/

    //disable_buffering();
    alarm(interval);
    char buffer[BUFF_SIZE];

    //char ch;
    while (timeout != 1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        no_input = 0;
        fputs(buffer, fout);
        fflush(fout);
        /*
        ssize_t r = read(STDIN_FILENO, &ch, 1);
        if (r == 0) {
            break;
        }
        if (r == -1){
            if(errno == EINTR) continue;
            perror("read");
            break;
        }
        no_input = 0;
        write(fd, &ch, 1);*/
    }

    fclose(fout);
    //enable_buffering();
    //close(fd);

    if (kill(child_id, SIGUSR2)) {
        perror("kill");
        exit(1);
    }
    /*
    int status;
    waitpid(child_id, &status, 0);
    */
   sigset_t wait_mask;
   sigemptyset(&wait_mask);
   while (!child_done) {
       sigsuspend(&wait_mask);
   }

    printf("Parinte: sfarsit executie!\n");
    return 0;
}