#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>// for execv and fork
#include <sys/types.h>
#include <sys/wait.h>//
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>  
#include "smallsh_functions.h"



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
        //printf("hello\n");
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



//***************NEEED TO IMPLEMENT THE $$

//referenced module: Processes and I/O
/**
stdin: File descriptor is 0. Defaults to reading from the terminal.
stdout: File descriptor is 1. Defaults to writing to the terminal.
stderr: File descriptor is 2. Defaults to writing to the terminal.
*/
void input_file(char* input_cmd){

  char file_name[250];
  strcpy(file_name, input_cmd);
  int filedescriptor = open(file_name, O_RDONLY );

  if(filedescriptor == -1){
    printf("Unable to open \"%s\"\n", file_name);
      fflush(stdout);
    exit(1);
  }


  printf("filedescriptor before dup2 == %d\n", filedescriptor); 
  fflush(stdout);
  //redirect the stdin to the input file
  int change = dup2(filedescriptor, 0);   //reading from terminal
  if(change == -1){
    perror("input dup2()");
  }



//can use int access(const char *pathname, int mode);
  /*    modes:

F_OK flag : Used to check for existence of file.
R_OK flag : Used to check for read permission bit.
W_OK flag : Used to check for write permission bit.
X_OK flag : Used to check for execute permission bit.
  */

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