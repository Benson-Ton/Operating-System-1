#include <sys/types.h>
#include <unistd.h> //exec
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <sys/wait.h> //wait functions
#include <stdbool.h>

#include "smallsh_functions.h"

//#define DEBUG

int main( ){
int childstatus;
int test = 0;
char *savepointer;
char user_input[2048]; // taking in inputs with maximum length

struct cmd_var *command_line = NULL;

    char str[] = "ls al bin < test > junk";

while(1){

printf(": ");
fgets(user_input, sizeof(user_input), stdin);//gets in the input from the keyboard

//getting rid of the next line at the end of the array
	for(int i = 0; i < sizeof(user_input); i++){
		if(user_input[i]== '\n'){user_input[i] = '\0';}
	}
 

command_line = read_commandline(user_input);

//printing for test
#ifdef DEBUG
printf("%d\n", command_line->num_arg);

for(int i = 0; i < command_line->num_arg ; i++){
	printf("ARgv:%s\n", command_line->argv[i]);
}

printf("output: %s\n", command_line->output_cmd);
printf("input: %s\n",command_line->input_cmd );
printf("background: %d\n",command_line->background);
printf("end of array: %s\n", command_line->argv[command_line->num_arg]);
#endif



//*** BUILT IN COMMANDS

if(strcmp("cd", command_line->argv[0]) == 0){


    if(command_line->argv[1] == NULL){
        chdir(getenv("HOME"));
       // chdir(getcwd("HOME"));
    }
    else if(command_line->argv[1] != NULL){
        chdir(getenv(command_line->argv[1]));
        
        if( chdir(command_line->argv[1]) != 0){
            perror("Error");
        }
    }


}

else if(strcmp("status",command_line->argv[0]) == 0){
    //**
    // write the foreground oricess with the wifexited of teh status of the child 
    //monitor child process module
    printf("exit status: %d\n", WIFEXITED(childstatus));
}

else if( strchr(command_line->argv[0], '#') ){}

else if(strcmp("exit",command_line->argv[0]) == 0){
    /*
The exit command exits your shell. It takes no arguments. 
When this command is run, 
your shell must kill any other processes or jobs that your shell has started 
before it terminates itself.


    */
exit(0); // exit success
}

else{



// keep in mind 
/*
    wc -> number of lines, word count, byte and characters count
*/



    //use the execvp function


//pid_t spawnpid = -5; 

//dont fork a process if the user is enters nothing 
if(strcmp("",command_line->argv[0])){
   
//** NOTE: for fork()
/*
fork() returns the value 0 in the child process, while the pid of the child 
process is returned by fork() in the parent process.  
This allows the child process and the parent process to diverge in their behavior.
*/

//refrenced module: Process API - Executing a New Program
    pid_t spawnpid = fork();
    
    //if fork is successful, the value of spawnpid will be 0 - in child, and child'S PID in parent
    fflush(stdout);


    switch(spawnpid){
        case -1:
            perror("fork failed");
            exit(1);                    //abnormal termination, minor problem in code
            break;
        case 0:
        //execute the code in this branch when the child is spawned
          //  printf("child (%d) is running command \n", getpid() );

            if(command_line->input_cmd != NULL){
               // printf("THIS IS THE INPUT \n");
                input_file(command_line->input_cmd);
                fflush(stdout);

                if(command_line->output_cmd != NULL){
                //    printf("THIS IS OUTPUT \n");
                    output_file(command_line->output_cmd);
                    fflush(stdout);
                }
            }
            else if(command_line->output_cmd != NULL){
               // printf("THIS IS OUTPUT \n");
                output_file(command_line->output_cmd);
                fflush(stdout);
            }

            //**NOTE: For execvp
            //The first element of this array, i.e., argv[0], must be the same as pathname
            //The last element of argv must be a null pointer.
       //    printf("length: %d \n",strlen(command_line->argv[0]));
            fflush(stdout);
            execvp(command_line->argv[0], command_line->argv);
            //execvp returns if only there is an error
           // fflush(stdout);
            perror("Unknown command");
            exit(2);                    //displayed for a major error in the code
            break; 
        default:
        // parent process
        //waiting for child termination
         //printf(" test >> %d \n", spawnpid);
            spawnpid = waitpid(spawnpid, &childstatus, 0);
        #ifdef DEBUG
            printf(" PARENT (%d): CHILD (%d) terminated. exiting\n", getpid(),spawnpid);
            fflush(stdout);
        #endif
            //exit(0);
            break;
    }
}

}






}








//pid_t fork();
//pid_t spawnpid = -5;
	//spawnpid = forK();




return 0;
}
