/*
* Benson Ton
* 5/16/22

*Assignment 4

Work Cited:
*Check substring exist in C from Stackoverflow
https://stackoverflow.com/questions/12784766/check-substring-exists-in-a-string-in-c

*Example code provided from class
https://replit.com/@cs344/65prodconspipelinec#main.c

*Exploration: Condition Variables
https://canvas.oregonstate.edu/courses/1883362/pages/exploration-condition-variables?module_item_id=21941927


*Exploration: Concurrency
https://canvas.oregonstate.edu/courses/1883362/pages/exploration-concurrency?module_item_id=21941923

*Exploration: Threads - Concepts & API
https://canvas.oregonstate.edu/courses/1883362/pages/exploration-threads-concepts-and-api?module_item_id=21941924

*Exploration: Synchronization for Concurrent Execution
https://canvas.oregonstate.edu/courses/1883362/pages/exploration-synchronization-for-concurrent-execution?module_item_id=21941925

*Exploration: Synchronization Mechanisms Beyond Mutex
https://canvas.oregonstate.edu/courses/1883362/pages/exploration-synchronization-mechanisms-beyond-mutex?module_item_id=21941926
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include "functions.h"

//size of the buffers
/*requirements: 
*	no more than 49 lines before stop 
*	no more than 1000 characters
*/ 
#define MAX_CHAR 1000
#define MAX_LINES 49
#define MAX_OUTPUT_CHAR 80



char text_buf[MAX_LINES][MAX_CHAR]; //input buffer
char sep_buf[MAX_LINES][MAX_CHAR]; //after line seperator
char plus_sign_buf[MAX_LINES][MAX_CHAR]; // after ++ are replaced


//boolean checkers
bool stop_input = false;



//**CITED: https://replit.com/@cs344/65prodconspipelinec
//**PIPELINE of producers and consumers consist of 3 threads example code
//intialize all 3 mutexes for the 3 shared buffers
pthread_mutex_t mutex_1, mutex_2, mutex_3 = PTHREAD_MUTEX_INITIALIZER;

/*Attempting a unbounded buffer*/

// global variables for text_buf
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
int prod_idx_1_col = 0;
// Index where the square-root thread will pick up the next item
int con_idx_1 = 0;
//initialize the conditon variable
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;



// global variables for sep_buf
int count_2 = 0;
// Index where the input thread will put the next item
int prod_idx_2 = 0;
// Index where the square-root thread will pick up the next item
int con_idx_2 = 0;
//initialize the conditon variable
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;




// global variables for plus_sign_buf
int count_3 = 0;
// Index where the input thread will put the next item
int prod_idx_3 = 0;
// Index where the square-root thread will pick up the next item
int con_idx_3 = 0;
//initialize the conditon variable
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;




//stores the character array into the main shared buffer
void put_buff_1(char* item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_1);
 
  // Put the string in the first buffer (text_buf)
	strcpy(text_buf[prod_idx_1],item);


  // Increment the index where the next item will be put.
  prod_idx_1 = prod_idx_1 + 1;
  count_1++;

  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_1);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);


}



//input thread functions 
//one of the functions that will be used for one of the thread (1/4)
void *input_thread(void *args)
{
	//used for transfering the lines from terminal to the main buffer
	char temp_buf[MAX_CHAR];
	
	//loop until STOP condition is triggered
	while(stop_input != true){

		//grabs the first line from the terminal
		fgets(temp_buf, sizeof(temp_buf), stdin);
		
			//exit out of the loop if it ecounters 'STOP'
		if(strcmp("STOP\n",temp_buf) == 0){
			put_buff_1(temp_buf);
			break;
		}
		//stores into text_buf
		put_buff_1(temp_buf);

	}
return NULL;
}


//grabs string from the 2D array; grabbing the characters from the terminal
char* get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while (count_1 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_1, &mutex_1);

	//need to allocate a specific length to get the string from the buffer
	int length  = strlen(text_buf[con_idx_1]);
	char* temp_c =  calloc(length,sizeof(char));
	strcpy(temp_c,text_buf[con_idx_1]);
  
  // Increment the index from which the item will be picked up
  con_idx_1 = con_idx_1 + 1;
  count_1--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
  // Return the item
  return temp_c;
}


//stores the character array into the main shared buffer
void put_buff_2(char* item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_2);
 
  // Put the string in the second buffer (sep_buf)
	strcpy(sep_buf[prod_idx_2], item);
  
  // Increment the index where the next item will be put.
  prod_idx_2 = prod_idx_2 + 1;
  count_2++;

  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_2);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
}

//replaces any line seperator (\n) with a space 
//will be used to pass through one of the threads (2/4)
void *line_seperator_thread(void *args)
{
	char *temp_c =  calloc(MAX_CHAR, sizeof(char*));
	
	//need to constantly loop until STOP condition is triggered
	while(1){

		//grabbing the string from the array and only expecting a maximum of 1000 characters
		temp_c = get_buff_1();
		
		if(strcmp("STOP\n",temp_c) == 0){
			//set flags to stop loops in other threads 
			stop_input = true;
			//need to store the STOP to pass on to the next shared buffer
			put_buff_2(temp_c);
			break;
		}
		//replace the newline with a sppace
		for(int i = 0; i < strlen(temp_c); i++){
			if(temp_c[i] == '\n'){temp_c[i] = ' ';}
		}
		//store into the next shared buffer (sep_buf)
		put_buff_2(temp_c);

	}

free(temp_c);
return NULL;
}


//grabs string from the 2D array; grabbing the characters from the share buffer
char* get_buff_2(){

  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_2);
  while (count_2 == 0)

    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_2, &mutex_2);

	//need to allocate a specific length to get the string from the buffer
	int length  = strlen(sep_buf[con_idx_2]);
	char* temp_c =  calloc(length,sizeof(char));
	strcpy(temp_c,sep_buf[con_idx_2]);
  
  //Increment the index from which the item will be picked up
  con_idx_2 = (con_idx_2 + 1) % MAX_LINES;
  count_2--;

  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
  // Return the item
  return temp_c;
}

//stores the character array into the main shared buffer
void put_buff_3(char* item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_3);
 
  // Put the string in the second buffer (plus_sign_buf)
  strcpy(plus_sign_buf[prod_idx_3],item);
  
  // Increment the index where the next item will be put.
  prod_idx_3 = prod_idx_3 + 1;
  count_3++;

  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_3);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
}


//replaces any '++' to a '^' 
//will be used to pass through one of the threads (3/4)
void *plus_sign_thread(void *args)
{
	char *temp_c =  calloc(MAX_CHAR, sizeof(char*));
	char* edited_string;
	//need to constantly loop until STOP condition is triggered
	while(1){


		temp_c = get_buff_2();

			if(strcmp("STOP\n",temp_c) == 0){
			//set flags to stop loops in other threads 
			stop_input = true;
			//need to store the STOP to pass on to the next shared buffer
			put_buff_3(temp_c);

			break;
		}
		
		//replace the ++ to a ^ then store it in one of the shared buffer plus_sign_buf
		edited_string = replaceplus(temp_c,"++","^");
		put_buff_3(edited_string);
	}

//freeing mem
free(edited_string);
free(temp_c);
return NULL;
}


//grabs string from the 2D array; grabbing the characters from the share buffer
char* get_buff_3(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_3);
  while (count_3 == 0)

    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_3, &mutex_3);

	//need to allocate a specific length to get the string from the buffer
	int length  = strlen(plus_sign_buf[con_idx_3]);
	char* temp_c =  calloc(length,sizeof(char));
	strcpy(temp_c,plus_sign_buf[con_idx_3]);

  // Increment the index from which the item will be picked up
  con_idx_3 = con_idx_3 + 1;
  count_3--;

  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
  // Return the item
  return temp_c;
}


//the last thread to be passed for output 
void *output_thread(void *args)
{
	bool flag = false;
	char* final_buf = calloc(MAX_CHAR, sizeof(char));
	char *temp_c = calloc(MAX_CHAR,sizeof(char));
	
	//need to constantly loop until STOP condition is triggered
	while(1){
		//obtain plus_sign_buf 
		
		temp_c = get_buff_3();

		if(strcmp("STOP\n",temp_c) == 0){
			//set flags to stop loops in other threads and printing
			stop_input = true;
			flag = true;
			break;
		}

		//put all of the lines into one character array
		strcat(final_buf,temp_c); 

	}

	//only run if the STOP condition has been initiated 
	if(flag){
		
		int counter = 0;
		int operation, length;

		//find the length of the buffer
		length = strlen(final_buf); 

		//set the total number of lines that needs to be printed
		operation = length/80;	

		//loop the number of lines that needs to be printed
		for (int i = 0; i < operation; i++)
		{
			for(int j = 0; j < 80; j++){
				//print up to 80 characters 
				printf("%c",final_buf[counter] );
				counter++;
			}
			//print the next line after 80 characters
			printf("\n"); 
		}
	}
	free(temp_c);
	free(final_buf);
	return NULL;
}



int main(int argc, char const *argv[])
{
	
	//points to a variable into which the ID of the new thread is written
	pthread_t input_t, line_sep_t, plus_sign_t,output_t;

//Exploration: Synchronization for Concurrent Execution
//https://canvas.oregonstate.edu/courses/1883362/pages/exploration-synchronization-for-concurrent-execution?module_item_id=21941925 
//creating threads  
    pthread_create(&input_t, NULL, input_thread, NULL);
    pthread_create(&line_sep_t, NULL, line_seperator_thread, NULL);
    pthread_create(&plus_sign_t, NULL, plus_sign_thread, NULL);
    pthread_create(&output_t, NULL, output_thread, NULL);


//Exploration: Synchronization Mechanisms Beyond Mutex
//https://canvas.oregonstate.edu/courses/1883362/pages/exploration-synchronization-mechanisms-beyond-mutex?module_item_id=21941926
    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    pthread_join(line_sep_t, NULL);
    pthread_join(plus_sign_t, NULL);
    pthread_join(output_t, NULL);

//Exploration: Synchronization for Concurrent Execution
//https://canvas.oregonstate.edu/courses/1883362/pages/exploration-synchronization-for-concurrent-execution?module_item_id=21941925 
//creating threads  
    //need to destroy mutex
    pthread_mutex_destroy(&mutex_1);
    pthread_mutex_destroy(&mutex_2);
    pthread_mutex_destroy(&mutex_3);

    // terminates the calling thread of our four threads
    pthread_exit(&input_thread);
    pthread_exit(&line_seperator_thread);
    pthread_exit(&plus_sign_thread);
    pthread_exit(&output_thread);



	return 0;
}
