#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>    // necesar pentru functia getpwuid
#include <grp.h>    // necesar pentru functia getgrgid
#include <limits.h> // necesar pentru PATH_MAX

#define BUFF_SIZE 4096

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Too many arguments!");
        return 2;
    }

    char* file_name = argv[1];
    char buffer[BUFF_SIZE];
    int bytes_read;
    int lowercase = 0, uppercase = 0;
    
    if(access(file_name, R_OK) != 0) {
        perror("File is not readable");
        return 3;
    }

    int input_fd = open(file_name, O_RDONLY);
    if(input_fd == -1){
        perror("Error opening file");
        return 1;
    }

    while((bytes_read=read(input_fd, buffer, BUFF_SIZE)) > 0) {
        for(int i=0;i<bytes_read;i++) {
            if((int)buffer[i] >= 'a' && (int)buffer[i] <= 'z') lowercase++;
            if((int)buffer[i] >= 'A' && (int)buffer[i] <= 'Z') uppercase++;
        }

    }

    close(input_fd);

    printf("%s:%d:%d\n", argv[1], uppercase, lowercase);


}