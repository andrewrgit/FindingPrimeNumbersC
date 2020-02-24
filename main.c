#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<libgen.h>


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


	return 0;
}



