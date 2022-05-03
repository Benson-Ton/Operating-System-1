#ifndef smallshell_h
#define smallshell_h

//At the time fork is executed this memory space is a duplicate of the parent process’s memory space. 
//This means that both the processes have the same variables and these variables have the same value.
//If fork fails it returns the value of -1.
// The most likely reasons are either that there are too many processes already 
//running in the system or that creating the new processes will cause the resource limit to exceed.

//**list of exec functions
//int execl(const char *pathname, const char *arg, ... /* (char  *) NULL */);
//int execlp(const char *filename, const char *arg, ... /* (char  *) NULL */);
//int execle(const char *pathname, const char *arg, ...  /*, (char *) NULL, char * const envp[] */);
//int execv(const char *pathname, char *const argv[]);
//int execvp(const char *filename, char *const argv[]);
//int execve(const char *pathname, char *const argv[], char *const envp[]);

        //**NOTE: For execvp
        //The first element of this array, i.e., argv[0], must be the same as pathname
        //The last element of argv must be a null pointer.

//**always run exec in the child isntead of parent cause parent is running commands

struct cmd_var{

	char *argv[512]; // a maximum of 512 arguments
	char *output_cmd; // output file command
	char *input_cmd; // input file command
	int num_arg; // keeping track of args, max of 512
	bool background;

};

struct cmd_var* read_commandline(char* lines);
void input_file(char* input_cmd);
void output_file(char* output_cmd);
//         if(!strcmp(in->input[0],"exit")){

//         }
//         else if(in->input[0][0] == '\0' || in->input[0][0] == '#'){
  
            
//         }
//         else if( !strcmp(in->input[0], "cd")){
            
//             //chdir(in->input[1]);
  
//         }
//         else if( !strcmp(in->input[0],"status") ){
        

//         }
//         else{
//   			//run other commandline
//   			//run execvp
//         }



#endif
