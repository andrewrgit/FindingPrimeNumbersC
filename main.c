#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[]){
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
	while ((option = getopt(argc, argv, "hn:s:b:i:o:")) != -1){
		printf("Loop started");
		switch(option){
			case 'h':
				printf("Help Message Here\n");
				hFlag = 1;
				return;
				break;
			case 'n':
				nFlag = 1;
				break;
			case 's':
				sFlag = 1;
				break;
			case 'b':
				bFlag = 1;
				break;
			case 'i':
				iFlag = 1;
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
	printf("oValue: %s", oValue);
	

	return 0;
}
