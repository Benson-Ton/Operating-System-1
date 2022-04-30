#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>// for execv and fork
#include <sys/types.h>
#include <sys/wait.h>//
#include <string.h>
#include "smallsh.h"



struct cmd_var* read_commandline(char* lines){

char *savepointer;
char *temp;
//char *token = strtok_r(lines, " ", &savepointer);
char *token;
int i = 0;

struct cmd_var* commandline = malloc(sizeof(struct cmd_var));

 //token = strtok_r(lines," ", &savepointer);

//    	    if(strcmp("<",token)){
//        		   	    // printf("print first : %s\n", token);
//        		token = strtok_r(NULL, " ", &savepointer);
//        	}
//        	else if(strcmp(">",token)){
//        		   	     //printf(" print second :%s\n", token);
//        		token = strtok_r(NULL, " ", &savepointer);
//        	}
   for (token = strtok_r(lines, " ", &savepointer);
       token != NULL;
       token = strtok_r(NULL, " ", &savepointer)){
  	     //printf("print in forloop : %s\n", token);
	  commandline->argv[i] = malloc(strlen(token)+1 * sizeof(char));
   	  strcpy(commandline->argv[i], token);
   	    
   	    if(strcmp("<",token)){
       		   	    // printf("print first : %s\n", token);
       		token = strtok_r(NULL, " ", &savepointer);
       	}
       	else if(strcmp(">",token)){
       		   	     //printf(" print second :%s\n", token);
       		token = strtok_r(NULL, " ", &savepointer);
		}
	  //token = strtok_r(NULL, " ", &savepointer);

 // > junk 
   	   		
   	i++;
 //   token = strtok_r(NULL, " ", &savepointer);
   }

   commandline->num_arg = i;

  	   //  printf("print in forloop : %s\n", token);
    //    while (token != NULL) {
    //     printf("%s\n", token);
    //     token = strtok(NULL, "< >");
    // }

  // token = strtok_r(lines, " ", &savepointer); //obtaining each line of file and storing it in a character array
   

 //  inputs->argv = malloc(strlen(movie_token) + 1);
  

   //strcpy(films->title,movie_token);
   
   // //year  
   // movie_token = strtok_r(NULL, ",", &savepointer); //clear token
   // films->year = atoi(movie_token);

   // //languages
   // movie_token = strtok_r(NULL, ",", &savepointer);

//return inputs;

}

// char * read_commandline(void)
// {
//   char *line = NULL;
//   ssize_t bufsize = 0; // have getline allocate a buffer for us

//   if (getline(&line, &bufsize, stdin) == -1){
//     if (feof(stdin)) {
//       exit(EXIT_SUCCESS);  // We recieved an EOF
//     } 
//     else  {
//       perror("readline");
//       exit(EXIT_FAILURE);
//     }
//   }

//   return line;
// }