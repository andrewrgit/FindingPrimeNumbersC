#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>

void killSigHandler();

int file;
char output[50];
char outputFilename[50];

int timeStartSec;
int timeStartMSec;
int currentTimeSec;
int currentTimeMSec;
int id;
int *shmPointer;

int main(int argc, char *argv[]){	
	
	signal(SIGINT, killSigHandler);

	
	printf("This is execed child\n");
	strcat(outputFilename, argv[3]);
	key_t key;
	key = ftok(".", 897);
	int shmid = shmget(key, 0, 0666);
	if(shmid < 0){
		perror("Exec could not get shared memmory");
	}

	shmPointer = shmat(shmid, NULL, 0);
	if(shmPointer == (void *) -1){
		perror("Exec could not attach shared memory");
	}

	file = open(argv[3], O_CREAT | O_RDWR | O_APPEND, 0666);	
	sprintf(output, "Child ID: %s started at %d seconds, %d miliseconds\n", argv[1], shmPointer[0], shmPointer[1]);
	write(file, output, strlen(output));
	close(file);
	output[0] = '\0';

	timeStartSec = shmPointer[0];
	timeStartMSec = shmPointer[1];

	errno = 0;
	id = strtol(argv[1], NULL, 10);
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
		if(abs(shmPointer[1] - timeStartMSec) > 100){
			isPrime = 0;
			shmPointer[id] = -1;
			break;
		}
	}
	if(isPrime == 1){
		shmPointer[id] = numToCheck;
	}


	printf("isPrime = %d\n", isPrime);
	
	if(isPrime == 1 && shmPointer[id] != -1){
		sprintf(output, "Child ID: %d found %d to be prime at %d seconds, %d mili seconds\n", id, numToCheck, shmPointer[0], shmPointer[1]);
	}
	else if(isPrime == 0){
		sprintf(output, "Child ID: %d found %d to NOT be prime at %d seconds, %d mili seconds\n", id, numToCheck, shmPointer[0], shmPointer[1]);	
	}
	else{
		sprintf(output, "Child ID: %d exited early at %d seconds, %d mili seconds\n", id, shmPointer[0], shmPointer[1]);
	}

	shmdt(shmPointer);
	
	file = open(argv[3], O_CREAT | O_RDWR | O_APPEND, 0666);	
	write(file, output, strlen(output));
	close(file);
	exit(0);
}

void killSigHandler(){
	sprintf(output, "Child ID: %d exited early at %d seconds, %d mili seconds\n", id, shmPointer[0], shmPointer[1]);
	file = open(outputFilename, O_CREAT | O_RDWR | O_APPEND, 0666);	
	write(file, output, strlen(output));
	close(file);
	exit(-1);
}
