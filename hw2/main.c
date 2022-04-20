#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "movies.h"

int main() {

   //find_largest_file(".");

  // getting the current directory 
   
   char cwd[PATH_MAX];
   if(getcwd(cwd, sizeof(cwd)) != NULL){
    //  printf("Current working dir: %s \n", cwd);
   }
   else{
      perror("getcwd() error");
      return EXIT_FAILURE;
   }



  // int condition;
  // condition = process_file(argc, *argv[1]);

   bool prompt = true;
   int response;
do{
   //prompt user for file choices
      printf("1. Select file to process \n");
      printf("2. Exit the program \n");
      scanf("%d", &response );

      //prompt user for more file choices
      if(response == 1){
         prompt = false;
         int second_response;

         printf("\nWhich file you want to process?\n\n");
         printf("Enter 1 to pick the largest file\n");
         printf("Enter 2 to pick the smallest file\n");
         printf("Enter 3 to smallest file\n");
         scanf("%d",&second_response);

         if(second_response == 1){
            find_largest_file(cwd);
         }

      }
      //exit program if the user wishes to
      else if(response == 2){
         prompt = false;
         return EXIT_SUCCESS;
      }
      else{
         printf("\nError, please choose a value of 1 or 2\n\n");
      }
   }
   while(prompt); // repeat if none of the choices are met




return 0;
}
