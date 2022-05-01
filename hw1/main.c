#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "movies.h"

int main ( int argc, char *argv[]) {

   //constant variables for processing file
   FILE * file_info;
   char const * filename = argv[1];
   char lines[1024];
   int counter = 0;

   if((file_info = fopen( filename, "r")) == NULL){
   printf("The file was unable to be opened and processed \n" );
   return 1;
   }

fgets( lines, sizeof(lines), file_info); //gets rid of the first line

char *movie_token;
char *savepointer;


//link list variables
struct movie *head = NULL;
struct movie *tail = NULL;
struct movie *films = NULL;

while(fgets( lines, sizeof( lines), file_info) ){

	//movie_token = strtok_r(lines, ",", &savepointer); //obtaining each line of file and storing it in a character array

//if list is empty (check if head and tail are null)
   // films = add_movie(first_line)
   // head = films
   //tail = films
   //count + 1

// if list is not empty
   // films = add_move(current line)
   // tail->next = films
   // tail = tail->next



//first node
if(head == NULL){
   films = add_movie(lines); // creates a node and populates it with movie information
   head = films;
   tail = films;
   counter++;
}

//creating the following nodes of the single linked list
else{
   films = add_movie(lines); // creates a node and populates it with movie information
   tail->next = films;
   tail = tail->next;
   counter++;
}
}
tail->next = NULL;

//traversing through the linked list
//printlist(head);
printf("%d movies have been parsed from %s file \n\n",counter, filename);

int response;
int condition = 1;


while(condition){
     response = choices_prompt();
     if(response < 1 || response > 4){printf("ERROR! Please choose a value between 1-4\n\n");}

      if(response == 1){
         movie_year_list(head);
      }
      if(response == 2){
         movie_rating_list(head, counter);
      }
      if(response == 3){
         condition = movie_language_list(head);
         
      }
      if(response == 4){
         printf("You have exited the program successfully \n");
         free(head);
         return EXIT_SUCCESS;
      }
}

// do{
//    response = choices_prompt();
//    if(response < 1 || response > 4){printf("ERROR! Please choose a value between 1-4\n\n");}
// }
// while(response < 1 || response > 4);


// switch(response){
//    case 1:
//       movie_year_list(head);
//       do{
//             response = choices_prompt();
//       if(response < 1 || response > 4){printf("ERROR! Please choose a value between 1-4\n\n");}
//       }
//       while(response < 1 || response > 4);
//       break;
//    case 2:
//       break;
//    case 3:
//       break;

//    //end program or reprompt
// }





//choices
/*
switch case

case1

*/


return 0;
}
