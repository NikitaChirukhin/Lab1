#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void child_process ();
void parent_process ();
void information (pid_t, char *);

int main (int argc, char * argv[]) {
    pid_t pid;

    if ((pid = fork()) == 0) {
        child_process();
    } else {
        parent_process();
    }

    exit(EXIT_SUCCESS);
}

void child_process () {
    pid_t pid = getpid();
    information(pid, "Child process start the game");
    sleep(4);
    exit(EXIT_SUCCESS);
}

void parent_process () {
    pid_t pid = getpid();
    int child_exit_status;
    information(pid, "Main work...");
    wait(&child_exit_status);
    information(pid, "For child process game is over");
    sleep(2);
    information(pid, "End work");
}

void information (pid_t pid, char * str) {
    time_t t = time(0);
    struct tm * timer = localtime(&t);
    printf("[%d:%d:%d][PID: %d]: %s\n", timer->tm_hour, timer->tm_min, timer->tm_sec, pid, str);
    fflush(stdout);
}
