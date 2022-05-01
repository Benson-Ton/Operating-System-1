#include "token-parser.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>

#define COMMAND_LINE_CHARS 2048
#define COMMAND_LINE_ARGS 512
//#define DEBUG
#define SIG

//this function searches a NULL-terminated list of tokens for the specified token
//it returns the position of the element if found and returns -1 if not found
int find_token(char* const * token_list, char const * token);

//this function removes n tokens starting at index loc from a NULL-terminated list of tokens
void remove_n_token(size_t num_toks, char* token_list[num_toks + 1], int const start, int const n);

void handle_SIGCHLD(int signo); //signal handler for when a child process finishes execution
void handle_SIGTSTP(int signo); //SIGSTP signal handler

bool foreground_mode = false; //this flag indicates whether the shell is running in foreground mode only
bool background_running = false; //this flag indicates whether the shell in running a background process
int lastChildStatus = -1; //this will store the termination status of the last foreground process.
int lastBackgroundChildStatus; //this will store the termination status of the last background process
pid_t lastBackgroundPID; //this will store the pid of the last background process that finished
			 //global so the SIGCHLD signal handler can access it
bool foreground_process_finished = true; //this flag indicates when a foreground process has finished
bool background_process_finished = false; //this flag indicates when a background process has finished 

int
main(int argc, char *argv[]){
	char **toks = NULL;
	size_t toks_size;
	size_t num_toks;
	char *cwd = NULL;
	char* homedir = getenv("HOME"); //HOME directory of the user

	//signal handling
	//SIGCHLD
	struct sigaction SIGCHLD_action = {0};
	SIGCHLD_action.sa_handler = handle_SIGCHLD;
	sigfillset(&SIGCHLD_action.sa_mask);
	SIGCHLD_action.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &SIGCHLD_action, NULL); //parent will call handle_SIGCHLD when a child process has finished

	//SIGINT
	struct sigaction SIGINT_action = { 0 };	
	SIGINT_action.sa_handler = SIG_IGN;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;
	sigaction(SIGINT, &SIGINT_action, NULL); //parent will ignore Ctrl-C */
	
	//SIGTSTP
	struct sigaction SIGTSTP_action = { 0 };
	SIGTSTP_action.sa_handler = handle_SIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &SIGTSTP_action, NULL); //parent will call handle_SIGTSTP when Ctrl-Z is sent

	#ifdef DEBUG
		printf("The pid of this process is: %d\n", getpid());
		fflush(stdout);

	#endif

	do{
		#ifdef DEBUG
			printf("The pid of this process is: %d\n", getpid());
			fflush(stdout);
		#endif

		//print a status message if a background process finished executing
		if (background_process_finished){
			//if a background process has been run, we must first check if the process ended normally or not
			if (WIFEXITED(lastBackgroundChildStatus)){
				printf("Background pid %d is done: exit status %d\n", lastBackgroundPID, WEXITSTATUS(lastBackgroundChildStatus));	
				fflush(stdout);

			}
			else{
				printf("Background pid %d is done: terminated by signal %d\n", lastBackgroundPID, WTERMSIG(lastBackgroundChildStatus));
				fflush(stdout);


			}			
			background_process_finished = false;

		}
		printf(": "); //print a command line prompt
		fflush(stdout);
		num_toks = readTokens(&toks, &toks_size); //tokenize the command line entry
		//ensure a command was entered before continuing
		if (num_toks > 0){
			if (foreground_mode){
				#ifdef DEBUG
					printf("in foreground mode\n");

				#endif
				//if & is found in foreground mode, remove it from the toks array
				if (strcmp(toks[num_toks-1], "&") == 0){
					#ifdef DEBUG
						printf("found '&'\n");
						printf("last tok: %s\n", toks[num_toks-1]);
						printf("num_toks: %d\n", num_toks);

					#endif
					remove_n_token(num_toks--, toks, num_toks-1, 1);
					#ifdef DEBUG
						printf("after removal\n");
						printf("last tok: %s\n", toks[num_toks-1]);
						printf("num_toks: %d\n", num_toks);

					#endif

				}
			
			}
			
			background_running = (strcmp(toks[num_toks-1], "&") == 0) ? true : false;
			pid_t child_pid = -1; //to keep track of parent and child processes
			bool i_redirection_set = false, o_redirection_set = false; //flags to specify if I/O has been redirected
			{
				int i = 0;
				char* loc; //pointer to instances of "$$"

				//go through each token and find instances of "$$"
				while (toks[i]){
					if ((loc = strstr(toks[i], "$$"))){
						#ifdef DEBUG
							printf("before strncpy: %s\n", toks[i]);
							fflush(stdout);

						#endif

						size_t len = strlen(toks[i]);
						char* string_to_format = malloc(sizeof(char)*len); //this will be passed to sprintf to format the string with pid
						strncpy(loc, "%d", 2);						
						strcpy(string_to_format, toks[i]);

						#ifdef DEBUG
							printf("after strncpy, before sprintf: %s\n", string_to_format);
							printf("toks[i] and string_to_format: %X, %X\n", toks[i], string_to_format);
							fflush(stdout);

						#endif
						sprintf(toks[i], string_to_format, getpid());
						free(string_to_format);

						#ifdef DEBUG
							printf("testing expansion: %s\n", toks[i]);
							fflush(stdout);

						#endif

					}
					i++;

				}
				int pos;
				//if there are input or output redirections, spawn a child to close stdout and/or stdin and open a regular file for redirection
				if (((pos = find_token(toks, ">")) != -1) || ((pos = find_token(toks, "<")) != -1)){
					#ifdef DEBUG
						printf("pos before the fork: %d\n", pos);
						printf("entered toks\n");
						fflush(stdout);

					#endif
					child_pid = fork(); //spawn a child
					//the child takes care of the redirection so that when the child exits, the parent's stdout and stdin are restored	
					if (child_pid == 0){
						do{
							//process output redirection
							if (strcmp(toks[pos], ">") == 0){
								//open a regular file and clone stdout in the child process
								int stdout_clone = open(toks[pos+1], O_CREAT | O_TRUNC | O_WRONLY, 00600);
								dup2(stdout_clone, STDOUT_FILENO);
								o_redirection_set = true;
								
								//remove the redirection and file tokens from the command buffer to pass it directly to exec if needed
								remove_n_token(num_toks, toks, pos, 2);
								num_toks -= 2;

								//check if there is also an input redirection to be processed
								pos = find_token(toks, "<");

							}
							//process input redirection
							else{
								#ifdef DEBUG
									printf("trying to open redirection file.\n");
									fflush(stdout);

								#endif
								//open a regular file and clone stdin in the child process
								int stdin_clone; 
								if ((stdin_clone = open(toks[pos+1], O_RDONLY)) == -1){
									//if the file can't be opened 
									err(1, "Could not open input file %s\n", toks[pos+1]);
									fflush(stdout);

								}
								dup2(stdin_clone, STDIN_FILENO);
								i_redirection_set = true;

								//remove the redirection and file tokens from the command buffer to pass it directly exec if needed
								remove_n_token(num_toks, toks, pos, 2);
								num_toks -= 2;

								//check if there is also an output redirection to be processed
								pos = find_token(toks, ">");

							}


						} while (pos != -1);


					}
					else{
						//the parent can resume control of the shell (parent) if the command 
						//is run in the background.
						//otherwise it must wait for the child to finish executing 
						if (background_running){
							lastBackgroundPID = child_pid;
							printf("Background pid is %d\n", child_pid);
							fflush(stdout);
							continue;

						}
						else{
							waitpid(child_pid, &lastChildStatus, 0);
							continue;

						}

					}

				}

			}
			if (strchr(toks[0], '#')){}

			else if (strcmp(toks[0], "cd") == 0)
			{ /* cd command -- shell internals */
			  if (num_toks == 1) 
			  {
			    if(chdir(homedir) == -1)
			    {
			      perror("Failed to change directory");
			    }
			  }
			  else if (chdir(toks[1]) == -1)
			  {
			    perror("Failed to change directory");
			  }
			}
			else if (strcmp(toks[0], "status") == 0){
				/*status command -- shell internals*/
				//first check that a foreground process has been run by the shell (i.e. not built-in commands)
				if (lastChildStatus == -1){
					printf("A foreground process has not been run yet. Exit status: 0\n");
					fflush(stdout);

				}
				else{
					//if a foreground process has been run, we must first check if the process ended normally or not
					if (WIFEXITED(lastChildStatus)){
						printf("Last foreground process exited normally with exit status: %d\n", WEXITSTATUS(lastChildStatus));	
						fflush(stdout);

					}
					else{
						printf("Last foreground process exited abnormally due to signal %d\n", WTERMSIG(lastChildStatus));
						fflush(stdout);


					}

				}

			}
			else if (strcmp(toks[0], "exit") == 0){
				/*exit command -- shell internals*/
				//do nothing for now

			}
			else // execute non built-in commands
			{ /* Default behavior: fork and exec */
								#ifdef DEBUG
					printf("I am a parent with pid: %d\n", getpid());
					printf("Check for previous child_pid definitions: %d\n", child_pid);
					fflush(stdout);

				#endif
				//check if the command should be run in the background
				if (background_running){
					background_running = true;
					//if there wasn't a child that forked off to handle redirections
					#ifdef DEBUG
						printf("background command detected\n");

					#endif
					if (child_pid == -1){
						//fork a child to redirect stdout and stdin to /dev/null
						child_pid = fork();
						#ifdef DEBUG
							printf("no prior redirections\n");
							printf("pid after forking: %d\n", getpid());
							fflush(stdout);

						#endif
						if (child_pid == 0){
							int stdin_clone = open("/dev/null", O_RDONLY);
							int stdout_clone = open("/dev/null", O_WRONLY);
							dup2(stdin_clone, STDIN_FILENO);
							dup2(stdout_clone, STDOUT_FILENO);
							
							printf("PID of new background process: %d\n", getpid());
							fflush(stdout);

						}
						else{
						//the child will proceed to execute the command in the background
						//so the parent can resume control of the shell	//signal handling

							lastBackgroundPID = child_pid;
							printf("Background pid is %d\n", child_pid);
							fflush(stdout);
							continue;		


						}

					}		
					//if there was a child that handled redirections, find out which redirections took place
					else{
						if (!o_redirection_set){
							#ifdef DEBUG
								printf("redirecting stdout to null\n");

							#endif
							int stdout_clone = open("/dev/null", O_WRONLY);
							dup2(stdout_clone, STDOUT_FILENO);

						}
						else if (!i_redirection_set){
							#ifdef DEBUG
								printf("redirecting stdin to null\n");

							#endif
							int stdin_clone = open("/dev/null", O_RDONLY);
							dup2(stdin_clone, STDIN_FILENO);

						}

					}

				}
				pid_t child_pid = fork();
				#ifdef DEBUG
					printf("pid returned from fork before command execution: %d\n", child_pid);


				#endif
				if (child_pid == 0)
					{ /* child executes this */
					#ifdef DEBUG
						printf("I am a child with pid: %d\n", getpid());
						fflush(stdout);

					#endif
					struct sigaction SIGTSTP_action = { 0 };
					SIGTSTP_action.sa_handler = SIG_IGN;
					sigfillset(&SIGTSTP_action.sa_mask);
					SIGTSTP_action.sa_flags = 0;
					sigaction(SIGTSTP, &SIGTSTP_action, NULL); //children will ignore SIGTSTP when Ctrl-Z is sent

					struct sigaction SIGINT_action = { 0 };	
					SIGINT_action.sa_handler = SIG_DFL; //kill foreground processes with Ctrl+C
					sigfillset(&SIGINT_action.sa_mask);
					SIGINT_action.sa_flags = 0;
					//background processes will ignore Ctrl+C
					if (background_running){
						SIGINT_action.sa_handler = SIG_IGN;
						remove_n_token(num_toks--, toks, num_toks-1, 1);

					}
					sigaction(SIGINT, &SIGINT_action, NULL); 

					execvp(toks[0], toks);
					//if exec fails, the child returns here
					err(errno, "failed to exec");
					fflush(stdout);
				}
				else{ 
					/* parent executes this */
					if (background_running){
						struct sigaction SIGTSTP_action = { 0 };
						SIGTSTP_action.sa_handler = SIG_IGN;
						sigfillset(&SIGTSTP_action.sa_mask);
						SIGTSTP_action.sa_flags = 0;
						sigaction(SIGTSTP, &SIGTSTP_action, NULL); //children will ignore SIGTSTP when Ctrl-Z is sent
						wait(&lastBackgroundChildStatus);	//parent must wait for command completion
						lastBackgroundPID = child_pid;
						background_process_finished = true;
						exit(EXIT_SUCCESS);

					}
					else{
						foreground_process_finished = false;
						wait(&lastChildStatus);	//parent must wait for command completion
						foreground_process_finished = true;

					}

				}

			}
			
			//return the child process forked before any I/O redirections occurred
			if (child_pid == 0){
				exit(EXIT_SUCCESS);

			}

		}

	} while ((num_toks == 0) || (strcmp(toks[0], "exit") != 0));


	return EXIT_SUCCESS;

}

int find_token(char* const * token_list, char const * token){
	int pos = 0;

	#ifdef DEBUG
		printf("token to find: %s\n", token);
		fflush(stdout);

	#endif
	
	//iterate through the list until NULL is found
	while (token_list[pos]){
		#ifdef DEBUG
			printf("loop: %d\n", pos);
			fflush(stdout);

		#endif
		if (strcmp(token_list[pos], token) == 0){
			#ifdef DEBUG
				printf("pos returned: %d\n", pos);
				fflush(stdout);

			#endif
			return pos;

		}
		pos++;

	}

	return -1;

}

void remove_n_token(size_t num_toks, char* token_list[num_toks + 1], int const start, int const n){
	int i;

	//copy everything to the right to the new token list
	i = start;
	while (token_list[i + n]){
		strcpy(token_list[i], token_list[i + n]);
		i++;

	}
	token_list[i] = NULL;

}

void handle_SIGCHLD(int signo){
	//kill all processes that finished executing in the background and became zombies
	//but ignore the processes still executing in the background
	pid_t child_pid;
	while ((child_pid = waitpid(-1, &lastBackgroundChildStatus, WNOHANG)) > 0){
		#ifdef DEBUG
			printf("Cleaned up pid: %d\n", lastBackgroundPID);

		#endif			
		background_process_finished = true;
		lastBackgroundPID = child_pid;

	}

}

void handle_SIGTSTP(int signo){
/*	while ((waitpid(-1, &lastChildStatus, WNOHANG) > 0) && !foreground_process_finished){
	
	}*/
	while (!foreground_process_finished && (waitpid(-1, &lastChildStatus, WNOHANG) == 0)){
		//spin until current foreground process finishes

	}
	if (!foreground_mode){
		char const * message = "\nEntering foreground-only mode (& is now ignored)\n: ";
		write(STDOUT_FILENO, message, 52);
		foreground_mode = true;

	}
	else{
		char const * message = "\nExiting foreground-only mode\n: "; 
		write(STDOUT_FILENO, message, 32);
		foreground_mode = false;

	}

}
