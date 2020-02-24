#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>


int main(int argc, char *argv[]){
	printf("This is execed child\n");
	key_t key;
	key = ftok(".", 897);
	int shmid = shmget(key, sizeof(int) * 2, 0666);
	if(shmid < 0){
		perror("Exec could not get shared memmory");
	}
	printf("Exec trying to attach memory\n");
	int *shmPointer = shmat(shmid, NULL, 0);
	if(shmPointer == (void *) -1){
		perror("Exec could not attach shared memory");
	}
	printf("ExecMem: [0]: %d\n", shmPointer[0]);

	exit(0);
}
