#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>

#define FILE "Producer_main.c"

int main()
{
	char * addr;
	int shmid;
	shmid = (shmget(2055, 32, 0666));

	if (shmid == -1) {
		printf("Can't open shared memory\n");
		exit(0);
	}

	if ((addr = shmat(shmid, NULL, 0)) == (char*)-1) {
		printf("Shmat err\n");
		exit(0);
	}



	while (1) {
		printf("%s\n", addr);
		sleep(1);
	}
	return 0;
}