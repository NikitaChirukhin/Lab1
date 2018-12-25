#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main()
{
	int fd[2];
	if (pipe(fd) < 0) {
		printf("Failed pipe open\n");
		exit(0);
	}

	time_t ttime;

	pid_t pid = fork();
	if (pid == 0) {	
		//child - source
		printf("CHILD: put sys time in pipe\n");

		close(fd[0]);
		time_t data = time(0);
		write(fd[1], &data, sizeof(time_t));
		printf("CHILD: process terminated\n");
		exit(0);
	}
	else	{	
		//PARENT - target
		sleep(1);
		printf("PARENT: get sys time from pipe\n");
		time_t parentTime;
 		read(fd[0], &parentTime, sizeof(parentTime));
		printf("PARENT: sys time is %s", ctime(&parentTime));
		printf("PARENT: the end of process\n");
		close (fd[0]);
	}
	return 0;
}