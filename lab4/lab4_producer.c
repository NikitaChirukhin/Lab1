#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>

#define FILE "Makefile"

int main() {

	char * addr;
	int shmid;
	shmid = (shmget(2055, 32, IPC_CREAT | 0666));
	if (shmid == -1) {
		printf("Can't create shared memory\n");
		exit(0);
	}

	if ((addr = shmat(shmid, NULL, 0)) == (char*)-1) {
		perror("Shmat err\n");
		exit(0);
	}

	time_t timer = time(0);
	time_t buft = timer;
	while (1) {
		timer = time(0);
		if (timer != buft) {
			buft = timer;
			sprintf(addr, "%s", ctime(&timer));
		}
	}
	return 0;
}