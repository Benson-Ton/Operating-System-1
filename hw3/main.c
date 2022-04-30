#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>// for execv and fork
#include <sys/types.h>
#include <sys/wait.h>//
#include <string.h>

#include "smallsh.h"

int main( ){

int test = 0;
char *savepointer;
char user_input[2048]; // taking in inputs with maximum length

struct cmd_var *command_line = NULL;

    char str[] = "ls al bin < test > junk";
 
    // Returns first token
 //   char* token = strtok(str, " ");
 
    // Keep printing tokens while one of the
    // delimiters present in str[].
    // while (token != NULL) {
    //   if(strcmp(">", token)){
    //     	printf("test > : %s\n", token);
    //     	token = strtok(NULL, " ");
    // 	}
    // 	else if(strcmp("<",token)){
    // 		printf("test < : %s\n", token);
    //     	token = strtok(NULL, " ");
    // 	}
    // }

//char *token = strtok_r(lines, " ", &savepointer);
// char *token;
// int i = 0;
//    for (token = strtok_r(str, " ", &savepointer);
//        token != NULL;
//        token = strtok_r(NULL, " ", &savepointer)){

 

//    	    if(strcmp("<",token)){
//        //		   	     printf("print first : %s\n", token);
//        		token = strtok_r(NULL, "<", &savepointer);
//        	}
//        	else if(strcmp(">",token)){
//      //  		   	     printf(" print second :%s\n", token);
//        		token = strtok_r(NULL, ">", &savepointer);

//        	}
//   	     printf("print after if : %s\n", token);


//    	  // commandline->argv[i] = 
   	
//    	i++;
   
//    }



do{

printf(": ");
fgets(user_input, sizeof(user_input), stdin);//gets in the input from the keyboard

//getting rid of the next line at the end of the array
	for(int i = 0; i < sizeof(user_input); i++){
		if(user_input[i]== '\n'){user_input[i] == '\0';}
	}


command_line = read_commandline(user_input);

//printing for test
printf("%d\n", command_line->num_arg);

for(int i = 0; i < command_line->num_arg ; i++){
	printf("ARgv:%s \n", command_line->argv[i]);
}

//check edge case later
	// if(input[0] == ''){	
	// 	test = 1;
	// }
	// if(test){
	// 	input[0] = '';
	// }

//now to parse the command line
//char *token = strtok_r(argv, " ", &savepointer);



}
while(1);




//pid_t fork();
//pid_t spawnpid = -5;
	//spawnpid = forK();




return 0;
}
