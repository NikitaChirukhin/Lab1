#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>

#define STORAGE_MAX 20
#define STORAGE_MIN 5

pthread_cond_t cv;
pthread_rwlock_t lock;

int change_storage(int * Ibuf, int num)
{
	int buf = Ibuf[0] + num;
	if (buf < STORAGE_MIN || buf > STORAGE_MAX) {
		return -1;
	}
	Ibuf[0] = buf;
	return 0;
}

void* producer(void *args) {
	int * Ibuf;
	int shmid;
	shmid = (shmget(2001, 2 * sizeof(int), IPC_CREAT | 0666));

	if (shmid == -1) {
		printf("Can't create shared memory\n");
		exit(0);
	}

	if ((Ibuf = shmat(shmid, NULL, 0)) == (int*)-1) {
		printf("Shmat error\n");
		exit(0);
	}

	pthread_rwlock_wrlock(&lock);
	Ibuf[0] = STORAGE_MAX;
	pthread_rwlock_unlock(&lock);

	while (1) {
		pthread_rwlock_wrlock(&lock);
		change_storage(Ibuf, 1);
		if (Ibuf[0] > STORAGE_MIN) {
			pthread_cond_signal(&cv);
		}
		pthread_rwlock_unlock(&lock);
		printf("PRODUCER: storage = %d\n", Ibuf[0]);
		sleep(1);
	}
	return 0;
}

int main() {
	pthread_t thread;
	int status;

	status = pthread_create(&thread, NULL, producer, NULL);
	if (status != 0) {
		printf("Main error: can't create thread, status = %d\n", status);
		exit(0);
	}
	if (0 != pthread_cond_init(&cv, NULL)) {
		printf("Can't pthread_cond_init");
		exit(0);
	}
	sleep(1);

	int * Ibuf;
	int shmid;
	shmid = (shmget(2018, 2 * sizeof(int), 0666));
	if (shmid == -1) {
		printf("Can't open shared memory\n");
		exit(0);
	}
	if ((Ibuf = shmat(shmid, NULL, 0)) == (int*)-1) {
		printf("Shmat err\n");
		exit(0);
	}
	while (1) {
		pthread_rwlock_rdlock(&lock);
		if (-1 != change_storage(Ibuf, -5))
			printf("CONSUMER: storage -5\n");
		pthread_rwlock_unlock(&lock);
	}

	return 0;
}