//Andrew Riebow
//4760, Project 2
//February 26, 2020

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

//Define function prototype
void killSigHandler();

//Declare globals
int file;
char output[90];
char outputFilename[50];

int timeStartSec;
int timeStartMSec;
int currentTimeSec;
int currentTimeMSec;
int id;
int *shmPointer;

int main(int argc, char *argv[]){	
	
	//Change signal handling
	signal(SIGINT, killSigHandler);

	
	//Put output filename from arguments into varible
	strcat(outputFilename, argv[3]);
	//Get shared memory and attach
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

	//Open a file to output when this child started
	file = open(argv[3], O_CREAT | O_RDWR | O_APPEND, 0666);	
	sprintf(output, "Child ID: %s started at %d seconds, %d miliseconds\n", argv[1], shmPointer[0], shmPointer[1]);
	write(file, output, strlen(output));
	close(file);
	output[0] = '\0';

	timeStartSec = shmPointer[0];
	timeStartMSec = shmPointer[1];

	errno = 0;
	id = strtol(argv[1], NULL, 10);
	int numToCheck = strtol(argv[2], NULL, 10);//Get number to check from arguments

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
	
	//Calculate Prime
	int isPrime = 1;
	int i;
	shmPointer[id] = 5;
	for(i=1;i<numToCheck;i++){
		if(numToCheck % i == 0 && i != 1 && i != numToCheck){	//If the number is divisible by only 1 and itself
			isPrime = 0;
			shmPointer[id] = numToCheck * -1;
		}
		if(abs(shmPointer[1] - timeStartMSec) > 100){ //if more than 100 ms has passed, exit
			printf("Child ID: %d has run for more than 100ms, exiting...\n", id);
			isPrime = 0;
			shmPointer[id] = -1;
			break;
		}
	}
	if(isPrime == 1){
		shmPointer[id] = numToCheck;
	}


	//Create output text for file based on whether the number was prime
	if(isPrime == 1 && shmPointer[id] != -1){
		sprintf(output, "Child ID: %d found %d to be prime at %d seconds, %d miliseconds\n", id, numToCheck, shmPointer[0], shmPointer[1]);
	}
	else if(isPrime == 0 && shmPointer[id] != -1){
		sprintf(output, "Child ID: %d found %d to NOT be prime at %d seconds, %d miliseconds\n", id, numToCheck, shmPointer[0], shmPointer[1]);	
	}
	else{
		sprintf(output, "Child ID: %d exited early at %d seconds, %d miliseconds\n", id, shmPointer[0], shmPointer[1]);
	}

	shmdt(shmPointer);//Detach memory
	
	//Output to file, create if it does not exist
	file = open(argv[3], O_CREAT | O_RDWR | O_APPEND, 0666);	
	write(file, output, strlen(output));
	close(file);
	printf("Child %d finished\n", id);
	exit(0);
}

//Function for handling signal
void killSigHandler(){
	sprintf(output, "Child ID: %d exited early at %d seconds, %d mili seconds\n", id, shmPointer[0], shmPointer[1]);
	file = open(outputFilename, O_CREAT | O_RDWR | O_APPEND, 0666);	
	write(file, output, strlen(output));
	close(file);
	exit(-1);
}
