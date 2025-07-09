#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>    // necesar pentru functia getpwuid
#include <grp.h>    // necesar pentru functia getgrgid
#include <limits.h> // necesar pentru PATH_MAX

#define num_chars 8

int main(int argc, char* argv[]) {
    if(argc>2){
        fprintf(stderr, "Too many arguments");
        return 1;
    }

    int input_fd = open(argv[1], O_RDONLY);
    off_t file_size = lseek(input_fd, 0, SEEK_END);
    if(file_size == -1) {
        perror("Error seeking the end of the file:");
        close(input_fd);
        return 2;
    }

    off_t start_pos = file_size - num_chars - 1;
    if( start_pos < 0) start_pos = 0;

    if(lseek(input_fd, start_pos, SEEK_SET) == -1) {
        perror("Error seeking file");
        close(input_fd);
        return 2;
    }

    char buffer[num_chars + 1];
    int bytes_read = read(input_fd, buffer, num_chars);
    if(bytes_read == -1) {
        perror("Error readinf file");
        close(input_fd);
        return 2;
    }

    buffer[bytes_read] = '\0';
    int numb = 0;

    for(int i=0;i<bytes_read;i++) {
        if((int)buffer[i] >= '0' && (int)buffer[i] <= '9') {
            numb=numb*10+((int)buffer[i] - '0');
        }
    }

    printf("%s:%d\n", argv[1], numb);

    return 0;

}