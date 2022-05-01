#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>// for execv and fork
#include <sys/types.h>
#include <sys/wait.h>//
#include <string.h>
#include <stdbool.h>
#include "smallsh.h"



struct cmd_var* read_commandline(char* lines){

char *savepointer;
bool temp = false;
char *token;
int i = 0;

struct cmd_var* commandline = malloc(sizeof(struct cmd_var));
commandline->background = false;

//check edge case when the user wants to press enter with no input
    if (strcmp(lines, "") == 0) {
        commandline->argv[0] = strdup(""); //
        //strcpy(commandline->argv[0],"");
        return commandline;
    }


   for (token = strtok_r(lines, " ", &savepointer);
      token != NULL;
      token = strtok_r(NULL, " ", &savepointer)){

      if(strcmp("&", token)==0){
       //token = strtok_r(NULL, " ", &savepointer);
        commandline->background = true;
        printf("hello\n");
      }

   	  if(strcmp("<",token) == 0){ //input file
          // take care of the redirection
       	//	   	     printf("print first : %s\n", token);
       	token = strtok_r(NULL, " ", &savepointer);
        commandline->input_cmd = malloc(strlen(token)+1 * sizeof(char));
        strcpy(commandline->input_cmd, token);
        temp = true;
       }
       else if(strcmp(">",token) == 0){
          // take care of the redirection
       //		   	     printf(" print second :%s\n", token);
       		token = strtok_r(NULL, " ", &savepointer);
          commandline->output_cmd = malloc(strlen(token)+1 * sizeof(char));
          strcpy(commandline->output_cmd, token); //output file
          temp = true;
		    }

       // printf("test>>>>%s\n",token );
        //delete the file commands from the argv
        if(temp != true){
          commandline->argv[i] = malloc(strlen(token)+1 * sizeof(char));
          strcpy(commandline->argv[i], token);
       	  i++;
        }

  }

   commandline->num_arg = i;


  return commandline;

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