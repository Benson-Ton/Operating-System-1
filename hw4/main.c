/*
 Benson Ton
 5/16/22

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include "functions.h"

#define MAX_INPUT_CHAR 1024
#define MAX_OUTPUT_CHAR 80

//size of the 3 buffers
char text_buf[MAX_INPUT_CHAR]; //input buffer
char sep_buf[MAX_INPUT_CHAR]; //after line seperator
char plus_sign_buf[MAX_INPUT_CHAR]; // after ++ are replaced
//char output_buf[MAX_OUTPUT_CHAR];

//boolean checkers
bool stop_process = false;
bool stop_input = false;

//index buffer trackers
//int index_input_buf = 0;

//intialize mutexes
pthread_mutex_t mutex_1, mutex_2, mutex_3, mutex_4 = PTHREAD_MUTEX_INITIALIZER;







//input thread functions 
//then checks whether the ascii values are within the boundaries
void *input_thread(void)
{
	char temp_buf[MAX_INPUT_CHAR];
do{
	//exit out of the loop if it ecounters 'STOP'
	if(strcmp("STOP\n",temp_buf) == 0){
		stop_input = true;
		printf("STOP PROCESS CONDITON HAS MET\n");
		return NULL;
	}

	if(stop_input == false){strcat(text_buf,temp_buf);}

	if(text_buf != NULL){

		for(int i = 0; i < strlen(text_buf); i++){
			if(  ((int)text_buf[i] > 32) || ((int)text_buf[i] < 126)  ){
				//printf("%d\n",(int)text_buf[i] );
				//printf("print char: %c \n", text_buf[i]);
				//printf("DOESNT WORK\n");
				//stop_input = false;
			//	index_input_buf++;
			}
			else{
				printf("FALSE\n");
				stop_input = true;
				stop_process = false;
			}
		}
	}
  }
  while(fgets(temp_buf, sizeof(temp_buf), stdin));

return NULL;
}


//separates the line 
//if not then dont store in buffer
void *line_seperator_thread(void)
{
//char terminate_process[10] = "STOP";

	for(int i = 0; i < strlen(text_buf); i++){
		if(text_buf[i] == '\n'){text_buf[i] = ' ';}
	}

   strcpy(sep_buf, text_buf);
return NULL;
}



void *plus_sign_thread(void)
{
	char* edited_string;

	//printf("\nOld string: %s\n", sep_buf);
	//fflush(stdout);
	edited_string = replaceplus(sep_buf, "++", "^");
	//printf("\nNew String: %s\n", result); 
	//fflush(stdout);
	strcpy(plus_sign_buf,edited_string);

	free(edited_string);
return NULL;
}


void *output_thread(void)
{
	//bool confirm_output = false;
	//bool loop = true;
	//char* output_strings = calloc(1024,sizeof(char));
	//int ending_char = MAX_OUTPUT_CHAR;
	int counter = 0;
	//int index = 0;

//printf("%d \n",strlen(output_strings) );

int temp, length;

length = strlen(plus_sign_buf);
temp = length/80;

// temp = floor( (sizeof(plus_sign_buf)/sizeof(plus_sign_buf[0])) /80);
// printf("TEMP IS: %d\n",temp );
// printf("STRLEN %d\n",strlen(plus_sign_buf) );
// int los = sizeof(plus_sign_buf)/sizeof(plus_sign_buf[0]);
// printf("SIZE OF %d\n", los );

for (int i = 0; i < temp; i++)
{

	for(int j = 0; j < 80; j++){


		printf("%c",plus_sign_buf[counter] );
		counter++;
	}
			//if(plus_sign_buf[counter] == NULL){break;}
	printf("\n");

}




	// if(strcmp("STOP\n",plus_sign_buf) == 0){
	// 	stop_input = true;
	// 	printf("STOP PROCESS CONDITON HAS MET IN OUTPUT\n");
	// 	return NULL;
	// }



	return NULL;
}



int main(int argc, char const *argv[])
{
	



	input_thread();
	line_seperator_thread();
	plus_sign_thread();
	output_thread();
	//printf("length of out buf:%d\n",strlen(output_buf) );
	//printf("%s\n", output_buf);
	//printf(" \n PLUS SIGN BUF: \n%s\n\n",plus_sign_buf );

	// for(int i = 0; i < strlen(plus_sign_buf); i++){
	// 	printf("%c", plus_sign_buf[i]);
	// }


	return 0;
}
