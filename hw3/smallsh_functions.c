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



/***************************************
definiton variables for $$ replacement
******************************************/
char *str = calloc(50, sizeof(char));
char pid_num_str[50];
bool append = false;
int counter = 0;
int dollar_counter = 0;
bool status = false;
int append_pid = 0;
int holder = 0;
bool pair_count = false;
int odd_dollar = 0;
int process_num = getpid(); //get the current process id
int length_num = sprintf(pid_num_str, "%d", process_num); // put the process id into a str variable 

  // printf("process ID:  %d \n", process_num);

   //check logic for mkdir command
   if(strcmp(commandline->argv[0], "") != 0){
    //printf("(string compare if statement)\n" );
    //make sure the arguement is not empty
     if(commandline->argv[1] != NULL){
      //printf("NULL if statement)\n" );
      //if the argument has $$ then move forward
      if(strstr(commandline->argv[1], "$$") != NULL){
        //printf("str str if statement\n");
      
      str = commandline->argv[1]; //allocate the arugment variable into the str variable for modification

    
        for (int j = 0; j < 50; j++){

          //  printf(">>>>>>>> %c\n",str[j] );
                //  printf("COUNTER: %d\n",counter);
            if(str[j] == '$'){counter++; odd_dollar++;}         

            //checks for how many pairs of $$ to be replaced
            if((counter % 2 == 0 && counter != 0)){       
              status = true;
              append_pid++;
      
              counter = 0;
             // pair_count = false;
            }
            
   
            //makes sure it is only replaced when there is a pair and start at the index before the second $
            if(status || pair_count){
              if(dollar_counter < length_num){
                 //  printf(" before the replacement: %s\n",str );
                 //  printf("INDEX IS :%d\n", j );
               //    printf("DOLLAR COUNTER BEFORE replacement :%d\n", dollar_counter );
                str[j-1] = pid_num_str[dollar_counter];
              //  printf(" after the replacement: %s\n",str );
                dollar_counter++;
              }
              //once it finishes the pid number then it is consider as one pair completed
              if(dollar_counter >= length_num){
              dollar_counter = 0;
              status = false;
              pair_count = true;
              holder++;
              }
            }
     //  printf("mod number: %d and limit number: %d \n", append_pid, holder);
          
          //once the desired pair and the completed pair matches then break the loop
          if(holder == append_pid && append_pid != 0){break;}

        }
        //checks if there is an odd number in order to append at the end
        if( (odd_dollar % 2) == 1 ){
      //    printf("DOLLAR %d", odd_dollar);
          append = true;
        }

        // append the $ if there is an odd amount
        char money = '$';
        if(append){strncat(str,&money,1);}
        commandline->argv[1] = str; //add it to the argument variable for the directory to be made

      }
    }
  }



//free(str);

//printf("directory name is: %s\n", commandline->argv[1]);


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


  //printf("filedescriptor before dup2 == %d\n", filedescriptor); 
 //fflush(stdout);
  //redirect the stdin to the input file
  int stdin_value = dup2(filedescriptor, 0);   //reading from the file now
  if(stdin_value == -1){
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

void output_file(char* output_cmd){

  char file_name[250];
  strcpy(file_name, output_cmd);
  int filedescriptor = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0640);

  if(filedescriptor == -1){
   // printf("Unable to open \"%s\"\n", file_name);
    perror("Unable to open");
    printf("\"%s\"\n", file_name);
    fflush(stdout);
    exit(1);
  }


  //printf("filedescriptor before dup2 for output== %d\n", filedescriptor); 
  //fflush(stdout);
  //redirect the stdin to the input file
  int stdin_value = dup2(filedescriptor, 1);   //writing to the file
  if(stdin_value == -1){
    perror("output dup2()");
  }


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