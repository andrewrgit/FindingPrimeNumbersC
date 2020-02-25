#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<libgen.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>

int main(int argc, char *argv[]){

	char *programName = basename(argv[0]);

	//Create bools for whether our options are selected or not
	int hFlag = 0;
	int nFlag = 0;
	int sFlag = 0;
	int bFlag = 0;
	int iFlag = 0;
	int oFlag = 0;
	//Supress getopt errors, allow for no input on arguments so they are set to the default
	opterr = 0;
	//Create default valeus for our options
	int nValue = 4;
	int sValue = 2;
	int bValue = 100;
	int iValue = 10;	
	char *oValue = "output.txt";
	
	int option;
	long tmp;
	char *eptr;

	while ((option = getopt(argc, argv, "hn:s:b:i:o:")) != -1){
		switch(option){
			case 'h':
				printf("Help Message Here\n");
				hFlag = 1;
				return 0;
				break;
			case 'n':
				nFlag = 1;
				errno = 0;
				tmp = strtol(optarg, eptr, 10);
				if(errno != 0 || tmp == 0){
					char *errorMessage[150];
					strcat(errorMessage, programName);
					strcat(errorMessage, ": Error: could not parse -n option argument, integer not found or n was 0");
					perror(errorMessage);
				}
				else{
					nValue = tmp;
				}
				printf("nValue: %d\n", nValue);
				break;
			case 's':
				sFlag = 1;
				errno = 0;
				tmp = strtol(optarg, eptr, 10);
				if(errno != 0 || tmp == 0){
					char *errorMessage[150];
					strcat(errorMessage, programName);
					strcat(errorMessage, ": Error: could not parse -s option argument, integer not found or s was 0");
					perror(errorMessage);
				}
				else{
					sValue = tmp;
				}
				printf("sValue: %d\n", sValue);
				break;
			case 'b':
				bFlag = 1;
				errno = 0;
				tmp = strtol(optarg, eptr, 10);
				if(errno != 0){
					char *errorMessage[150];
					strcat(errorMessage, programName);
					strcat(errorMessage, ": Error: could not parse -b option argument, integer not found");
					perror(errorMessage);
				}
				else{
					bValue = tmp;
				}
				printf("bValue: %d\n", bValue);
				break;
			case 'i':
				iFlag = 1;
				errno = 0;
				tmp = strtol(optarg, eptr, 10);
				if(errno != 0 || tmp == 0){
					char *errorMessage[150];
					strcat(errorMessage, programName);
					strcat(errorMessage, ": Error: could not parse -i option argument, integer not found or i was 0");
					perror(errorMessage);
				}
				else{
					iValue = tmp;
				}
				printf("iValue: %d\n", iValue);
				break;
			case 'o':
				if(optarg != NULL){
					oFlag = 1;
					oValue = optarg;
				}
				break;
		}
		//printf("%s\n", optarg);
	}
	printf("oValue: %s\n", oValue);
	printf("ovalue second place: %c\n", oValue[1]);

	clock_t before = clock();


	key_t key;
	key = ftok(".", 897);
	int shmid = shmget(key, sizeof(int) * nValue, 0666 | IPC_CREAT);
	if(shmid < 0){
		char *errorMessage[150];
		strcat(errorMessage, programName);
		strcat(errorMessage, ": Error: Failed to create shared memory");
		perror(errorMessage);
	}
	int *shmPointer = shmat(shmid, NULL, 0);
	if (*shmPointer == (void *) -1){
		char *errorMessage[150];
		strcat(errorMessage, programName);
		strcat(errorMessage, ": Error: Failed to attach shared memory");
		perror(errorMessage);
	}
	
	shmPointer[0] = 5;
	shmPointer[1] = 9;

	char bString[10];
	char idString[10];
	int numOfChildren = 0;

	int i;
	for(i = 0; i < nValue; i ++){	
		pid_t childPid;
		if(numOfChildren >= sValue){
			wait(NULL);
			numOfChildren--;
		}
		childPid = fork();
		

		bString[0] = '\0';
		idString[0] = '\0';
		bValue = bValue + iValue;
		sprintf(idString, "%d", i + 2);
		sprintf(bString, "%d", bValue);
		if(childPid == 0){
		printf("This is the child process\n");
		char *paramList[] = {"./prime", idString, bString, NULL};
		execv("prime", paramList);
		exit(0);
		}
		else if(childPid < 0){
			char *errorMessage[150];
			strcat(errorMessage, programName);
			strcat(errorMessage, ": Error: fork() failed to create child");
			perror(errorMessage);
		}
		else{
			numOfChildren++;
		}
	}
	printf("Parent to wait for children...\n");

	pid_t allChildrenPid;
	int status = 0;
	while ((allChildrenPid = wait(&status)) > 0){
		numOfChildren--;
	}
	printf("shmPointer 2,3,4: %d, %d, %d\n", shmPointer[2],shmPointer[3],shmPointer[4]);
	if((shmdt(shmPointer)) < 0){
		char *errorMessage[150];
		strcat(errorMessage, programName);
		strcat(errorMessage, ": Error: Failed to detach shared memory from parent");
		perror(errorMessage);
	}
	if((shmctl(shmid, IPC_RMID, 0)) < 0){
		char *errorMessage[150];
		strcat(errorMessage, programName);
		strcat(errorMessage, ": Error: Failed to delete shared memory");
		perror(errorMessage);	
	}
	int j = 0;
	int d;
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);
	printf("TIME BEFORE: %d\n", time.tv_sec);
	for(j = 0; j < 1000000; j++){
	for(d = 0; d < 1000; d++){

		}
	}
	clock_t after = clock();
	clock_t difference = after - before;
	int msec = difference * 1000 / CLOCKS_PER_SEC;
	printf("Time taken %d seconds %d milliseconds\n",
	  msec/1000, msec%1000);
	return 0;
}



