//keygen
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ALPHABET 27

int main(int argc, char* argv[])
{

	int keygen;
	srand(time(NULL));

	//if less than 2 arguments then return error
	if(argc != 2){

		printf("Invalid arguments\n");
		return EXIT_FAILURE;
	}
	else{
		//set alphabet array with space
		char letters[ALPHABET] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

		//convert the second command line argument to an integer
		int input = atoi(argv[1]);

		//create an array based on the number of characters from the commandline and plus 1 for the newline
		char * final_key = calloc(input, sizeof(char) + 1);
		int i;

		for(i = 0; i < input; i++){

			//generate random key from 0 to 26
			keygen = rand() % 27;	
			//add to the final key aray based on the generated letter 
			final_key[i] = letters[keygen]; 
		}
	//print out the final key 
	printf("%s\n", final_key);
	free(final_key);
	//flush the output
	fflush(stdout);
	}
	return 0;
}