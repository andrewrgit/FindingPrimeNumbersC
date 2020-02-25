#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc, char *argv[]){
	printf("This is execed child\n");

	key_t key;
	key = ftok(".", 897);
	int shmid = shmget(key, 0, 0666);
	if(shmid < 0){
		perror("Exec could not get shared memmory");
	}

	int *shmPointer = shmat(shmid, NULL, 0);
	if(shmPointer == (void *) -1){
		perror("Exec could not attach shared memory");
	}

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
	}
	if(isPrime == 1){
		shmPointer[id] = numToCheck;
	}
//	int j = 0;
//	int d;
//	for(j = 0; j < 1000000; j++){
//	for(d = 0; d < 1000; d++){
//
	//	}
	//}
	printf("isPrime = %d\n", isPrime);
	
	if((shmdt(shmPointer)) < 0){
		char *errorMessage[150];
		strcat(errorMessage, basename(argv[0]));
		strcat(errorMessage, ": Error: Failed to detach shared memory from parent");
		perror(errorMessage);
	}
	int file;
	char output[50];
	if(isPrime == 1){
		sprintf(output, "Child ID: %d found %d to be prime\n", id, numToCheck);
	}
	else{
		sprintf(output, "Child ID: %d found %d to NOT be prime\n", id, numToCheck);	
	}
	file = open(argv[3], O_CREAT | O_RDWR | O_APPEND, 0666);	
	write(file, output, strlen(output));
	close(file);
	
	exit(0);
}
