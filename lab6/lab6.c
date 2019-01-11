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

pthread_mutex_t mutex;
pthread_cond_t cv;

int change_storage(int * addr, int num)
{
		int buf = addr[0] + num;
        if(buf < STORAGE_MIN || buf > STORAGE_MAX){
                return -1;
        }
        addr[0] = buf;
        return 0;
}

void* producer(void *args) {
        int * addr;
        int shmid;
	key_t semkey = ftok("/tmp", 'a');
        shmid = (shmget(semkey, 2*sizeof(int), IPC_CREAT | 0666));

        if( shmid == -1 ){
                printf("Can't create shared memory\n");
                exit(0);
        }

        if((addr = shmat( shmid, NULL, 0 )) == (int*)-1){
                printf("Shmat err\n");
                exit(0);
        }
        pthread_mutex_lock(&mutex);
        addr[0] = STORAGE_MAX;
        pthread_mutex_unlock(&mutex);

        while(1){
                pthread_mutex_lock(&mutex);
                        change_storage(addr, 1);
                if(addr[0] > STORAGE_MIN){
                        pthread_cond_signal(&cv);
                }
                pthread_mutex_unlock(&mutex);
                printf("PRODUCER: storage = %d\n", addr[0]);
                sleep(1);
        }
        return 0;
}

int main() {
        pthread_t thread;
        int status;

        pthread_mutex_init(&mutex, NULL);

        status = pthread_create(&thread, NULL, producer, NULL);
        if (status != 0) {
                printf("Main error: can't create thread, status = %d\n", status);
                exit(0);
        }
        if(0 != pthread_cond_init(&cv, NULL)){
                printf("Can't pthread_cond_init");
                exit(0);
        }
        sleep(1);

        int * addr;
        int shmid;
	key_t semkey = ftok("/tmp", 'a');
        shmid = (shmget(semkey, 2*sizeof(int), 0666));
        if( shmid == -1 ){
                printf("Can't open shared memory\n");
                exit(0);
        }
        if((addr = shmat( shmid, NULL, 0 )) == (int*)-1){
                printf("Shmat err\n");
                exit(0);
        }
        while(1){
                pthread_mutex_lock(&mutex);
                pthread_cond_wait(&cv, &mutex);
                        if(-1 != change_storage(addr, -5))
                        printf("CONSUMER: storage -5\n");
                pthread_mutex_unlock(&mutex);
                //sleep(1);
        }

        pthread_mutex_destroy(&mutex);
        return 0;
}