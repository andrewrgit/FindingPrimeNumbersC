//Andrew Riebow
//4760, Project 2
//February 26, 2020

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
#include<signal.h>
#include<fcntl.h>

//Declare function prototypes
void signalHandler(int signal);
void updateTime(struct timespec time, int *shmPointer, int startingSec, long startingMSec);

//Declare global variables for use in signal handler
int *shmPointer;
int shmid;

int main(int argc, char *argv[]){

	//Set ctrl-c interrupt to be handled by signalHandler function
	signal(SIGINT, signalHandler);

	//Get basename of the program for use in error messages
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
	int iValue = 3;	
	char *oValue = "output.txt";
	
	int option;
	long tmp;
	char *eptr;

	//Get options, just like in the first project
	while ((option = getopt(argc, argv, "hn:s:b:i:o:")) != -1){
		switch(option){
			case 'h':
				printf("oss calculates whether certain numbers are prime or not.\n"
					"\tOptions:\n"
					"\t(-h): Display help message and exit\n"
					"\t(-n x): Indicate the maximum total child processes oss will ever create. (Default 4)\n"
					"\t(-s x): Indicate the number of children allowed to exist in the system at the same time. (Default 2)\n"
					"\t(-b x): Start of the sequence of numbers to be tested for primality. (Default 100)\n"
					"\t(-i x): Increment between numbers that we test. (Default 3)\n"
					"\t(-o filename): Output file for logging. (Default output.txt)");

				hFlag = 1;
				return 0;
				break;
			case 'n':
				nFlag = 1;
				errno = 0; //Reset errno to check if an error occurs right after
				tmp = strtol(optarg, eptr, 10); //To preserve our default, put the value of strtol in a temporary variable in case it errors
				if(errno != 0 || tmp == 0){
					char *errorMessage[150];
					strcat(errorMessage, programName);
					strcat(errorMessage, ": Error: could not parse -n option argument, integer not found or n was 0");
					perror(errorMessage);
				}
				else{
					nValue = abs(tmp);
				}
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
					sValue = abs(tmp);
					if(sValue > 20) sValue = 20;
				}
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
					bValue = abs(tmp);
				}
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
					iValue = abs(tmp);
				}
				break;
			case 'o':
				if(optarg != NULL){
					oFlag = 1;
					oValue = optarg;
				}
				break;
		}
		
	}

	//Create shared memory
	key_t key; //Create a key for the shared memory
	key = ftok(".", 897); //Create a unique key
	shmid = shmget(key, sizeof(int) * nValue, 0666 | IPC_CREAT); //Allocate memory
	if(shmid < 0){
		char *errorMessage[150];
		strcat(errorMessage, programName);
		strcat(errorMessage, ": Error: Failed to create shared memory");
		perror(errorMessage);
	}
	shmPointer = shmat(shmid, NULL, 0); //Attach the memory
	if (shmPointer == (void *) -1){
		char *errorMessage[150];
		strcat(errorMessage, programName);
		strcat(errorMessage, ": Error: Failed to attach shared memory");
		perror(errorMessage);
	}
	
	//Initialize shared mem locations to 0
	int j;
	for(j = 2; j < nValue + 2; j++){
		shmPointer[j] = 0;
	}

	//Set up our clock
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);

	int startingTimeSec = time.tv_sec;
	long startingTimeMSec = time.tv_nsec / 1000000;
	int afterTimeSec = time.tv_sec - startingTimeSec;
	long afterTimeMSec = time.tv_nsec / 1000000 - startingTimeMSec;

	shmPointer[0] = afterTimeSec; //Set the seconds to the first array location
	shmPointer[1] = afterTimeMSec; //Set the miliseconds to the second array location


	char bString[10]; //Create variables for b and id to be passed as paramters because they must be strings
	char idString[10];
	int numOfChildren = 0; //Create a counter for number of children currently running

	//Main loop for creating child processes
	int i;
	for(i = 0; i < nValue; i ++){	
		pid_t childPid;
		if(numOfChildren >= sValue){ //If we have max number of children specified from the options
			wait(NULL); //Wait until any child exits
			numOfChildren--;//Decrement number of child processes counter
		}
		updateTime(time, shmPointer, startingTimeSec, startingTimeMSec); //Function to update time in shared memory, right before we create a new process so they can get their start time
		 
		childPid = fork(); //Create child process
		
		//reset string variables to empty
		bString[0] = '\0';
		idString[0] = '\0';
		sprintf(idString, "%d", i + 2);
		sprintf(bString, "%d", bValue);

		//Child process
		if(childPid == 0){

		char *paramList[] = {"./prime", idString, bString, oValue, NULL};
		execv("prime", paramList);
		exit(0);
		}
		else if(childPid < 0){ //If fork failed
			char *errorMessage[150];
			strcat(errorMessage, programName);
			strcat(errorMessage, ": Error: fork() failed to create child");
			perror(errorMessage);
		}
		else{
			numOfChildren++; //Parent process
		}
		bValue = bValue + iValue; //Increase the next number to be given to process by the increment option value

		if(shmPointer[0] >= 2 && (shmPointer[1] - startingTimeMSec) > 0){ //If it has been more than 2 seconds in time, kill children, exit parent process, and clear memory
			printf("Parent execution time exceeded 2 seconds, exiting...\n");
			shmdt(shmPointer);
			shmctl(shmid, IPC_RMID, 0);
			kill(0, SIGKILL);
			exit(-1);
		}
	}

	//Wait in a loop for children to finish, check max time again
	pid_t allChildrenPid;
	int status = 0;
	while ((allChildrenPid = wait(&status)) > 0){
		numOfChildren--;
		if(shmPointer[0] >= 2 && (shmPointer[1] - startingTimeMSec) > 0){ //If it has been more than 2 seconds in time, kill children, exit parent process, and clear memory
			printf("Parent execution time exceeded 2 seconds, exiting...");
			shmdt(shmPointer);
			shmctl(shmid, IPC_RMID, 0);
			kill(0, SIGKILL);
			exit(-1);
		}
	}
	

	updateTime(time, shmPointer, startingTimeSec, startingTimeMSec);
	
	//Output numbers to log file
	char output[200];
	int file = open(oValue, O_CREAT | O_RDWR | O_APPEND, 0666); //Open file
	sprintf(output, "\nPrime Numbers: ");
	write(file, output, strlen(output));
	
	//Iterate over the shared memory array, and depending on what the number is, output it in a specific section
	for(i = 2; i < nValue + 2; i++){
		output[0] = '\0';
		if(shmPointer[i] >= 0){
			sprintf(output, "%d ", shmPointer[i]);
			write(file, output, strlen(output));
		}
	}
	write(file, "\nNon Prime Numbers: ", strlen("\nNon Prime Numbers: "));
	for(i = 2; i < nValue + 2; i++){
		output[0] = '\0';
		if(shmPointer[i] < -1){
			sprintf(output, "%d ", abs(shmPointer[i]));
			write(file, output, strlen(output));
		}
	}
	output[0] = '\0';
	write(file, "\nNumber of failed child processes: ", strlen("\nNumber of failed child processes: "));
	int failedCount = 0;
	for(i = 2; i < nValue + 2; i++){
		if(shmPointer[i] == -1){
			failedCount++;
		}
	}
	sprintf(output, "%d", failedCount);
	strcat(output, "\n\n");
	write(file, output, strlen(output));
	close(file);

	//Detach and delete shared memory
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
	printf("Parent finished\n");
	return 0;
}

//Function for handling ctrl-c signal
void signalHandler(int signal){
	printf("Canceled with ctrl-c, stopping children...\n");
	kill(0, SIGINT);

	shmdt(shmPointer);
	shmctl(shmid, IPC_RMID, 0);
	exit(-1);
	
}

//Function to update the clock in shared memory with the current time. Called periodically throughout program
void updateTime(struct timespec time, int *shmPointer, int startingSec, long startingMSec){
	clock_gettime(CLOCK_MONOTONIC, &time);
	shmPointer[0] = time.tv_sec - startingSec;
	shmPointer[1] = time.tv_nsec / 1000000 - startingMSec;
}

