#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_CMD_ARGS 10

// Function to create FIFO between commands
void create_fifo(const char* fifo_name) {
    if (mkfifo(fifo_name, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo failed");
            exit(EXIT_FAILURE);
        }
    }
}

// Function to execute a command with FIFO redirection
void exec_command(char* cmd, char* input_fifo, char* output_fifo) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        int input_fd = -1, output_fd = -1;

        if (input_fifo != NULL) {
            input_fd = open(input_fifo, O_RDONLY);
            if (input_fd == -1) {
                perror("Failed to open input FIFO");
                exit(EXIT_FAILURE);
            }
            dup2(input_fd, STDIN_FILENO); // Redirect input
            close(input_fd);
        }

        if (output_fifo != NULL) {
            output_fd = open(output_fifo, O_WRONLY);
            if (output_fd == -1) {
                perror("Failed to open output FIFO");
                exit(EXIT_FAILURE);
            }
            dup2(output_fd, STDOUT_FILENO); // Redirect output
            close(output_fd);
        }

        // Split command into arguments
        char* args[MAX_CMD_ARGS];
        char* token = strtok(cmd, " ");
        int i = 0;
        while (token != NULL && i < MAX_CMD_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Execute the command
        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <cmd1> <cmd2> ... <cmdN>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = argc - 1; // Number of commands
    char* commands[n];

    // Store commands
    for (int i = 0; i < n; i++) {
        commands[i] = argv[i + 1];
    }

    // Create FIFOs and set up pipes
    char input_fifo[256], output_fifo[256];
    for (int i = 0; i < n - 1; i++) {
        snprintf(input_fifo, sizeof(input_fifo), "fifo_in_%d", i);
        snprintf(output_fifo, sizeof(output_fifo), "fifo_out_%d", i);
        
        create_fifo(input_fifo);
        create_fifo(output_fifo);
    }

    // Execute each command with FIFO redirection
    for (int i = 0; i < n; i++) {
        char* input_fifo = (i == 0) ? NULL : argv[i - 1];  // First command has no input FIFO
        char* output_fifo = (i == n - 1) ? NULL : argv[i]; // Last command has no output FIFO

        exec_command(commands[i], input_fifo, output_fifo);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    // Cleanup: remove FIFOs
    for (int i = 0; i < n - 1; i++) {
        snprintf(input_fifo, sizeof(input_fifo), "fifo_in_%d", i);
        snprintf(output_fifo, sizeof(output_fifo), "fifo_out_%d", i);
        unlink(input_fifo);
        unlink(output_fifo);
    }

    return EXIT_SUCCESS;
}
