#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    const char *cmd = "cut --fields 1,3,4,6 -d: --output-delimiter=\"--\" /etc/passwd";

    execlp("./MyCall_System", "MyCall_System", cmd, NULL);

    perror("execlp");
    return EXIT_FAILURE;
}
