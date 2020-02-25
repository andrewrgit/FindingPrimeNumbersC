#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>

int main(int argc, char *argv[]){
	printf("This is execed child\n");
	printf("what\n");

	key_t key;
	key = ftok(".", 897);
	int shmid = shmget(key, 0, 0666);
	if(shmid < 0){
		perror("Exec could not get shared memmory");
	}

	printf("Exec trying to attach memory\n");
	int *shmPointer = shmat(shmid, NULL, 0);
	if(shmPointer == (void *) -1){
		perror("Exec could not attach shared memory");
	}

	printf("ExecMem: [0]: %d\n", shmPointer[0]);
	printf("Exec argv[0], argv[1]: %s,%s\n", argv[0], argv[1]);

	errno = 0;
	int id = strtol(argv[1], NULL, 10);
	int numToCheck = strtol(argv[2], NULL, 10);

	if(numToCheck == 0){
		perror("could not strtol argv[1]");
		exit(-1);
	}
	if (id == 0){
		perror("could not strtol argv[2]");
		exit(-1);
	}
	if(errno != 0){
		perror("Child Process failed to find prime number");
		exit(-1);
	}
	printf("strtol argv[1], %d\n", id);
	printf("strtol argv[2], %d\n", numToCheck);
	
	//Calculate Prime
	int isPrime = 1;
	int i;
	shmPointer[id] = 5;
	printf("Checking\n");
	for(i=1;i<numToCheck;i++){
		if(numToCheck % i == 0 && i != 1 && i != numToCheck){	
			isPrime = 0;
			shmPointer[id] = numToCheck * -1;
		}
		printf("OutLoop\n");
	}
	if(isPrime == 1){
		shmPointer[id] = numToCheck;
	}
	printf("isPrime = %d\n", isPrime);
	exit(0);
}
