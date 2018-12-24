#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO "./fifo_file"
#define BUFFER_SIZE 80

void write_time(char *);

int main (int argc, char * argv[]) {
    pid_t pid;
    char * buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    mkfifo(FIFO, 0777);

    pid = fork();

    if (pid == 0) {
        int fd = open(FIFO, O_WRONLY);
        write_time(buffer);
        write(fd, buffer, BUFFER_SIZE);
        close(fd);
    } else {
        int fd = open(FIFO, O_RDONLY);
        read(fd, buffer, BUFFER_SIZE);
        printf("Receive from other process: %s\n", buffer);
        close(fd);
    }
}

void write_time(char * str) {
    time_t timer = time(0);
    struct tm timeval = *localtime(&timer);

    sprintf(str, "[%.2d:%.2d:%.2d]\n", timeval.tm_hour, timeval.tm_min, timeval.tm_sec);
}
