#include "movies.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE  10000000

int counter = 0;

void printList(struct movie* n)
{
    while (n != NULL) {
        printf(" %s\n ", n->title);
        n = n->next;
    }
}


/******************************************
**                swap_title
** swaps the title between two arrays
**
********************************************/
void swap_title(char ** title_1, char ** title_2){
   char * temp = *title_1;
   *title_1 = *title_2;
   *title_2 = temp;
};

/******************************************
**                swap_year
** swaps the year between two arrays
**
********************************************/
void swap_year(int* year_1, int* year_2){
   int temp = *year_1;
   *year_1 = *year_2;
   *year_2 = temp;
};


/******************************************
**                bubbleSort
** sort the arrays in sequential order of the years
** citation: algorithm is from stackoverflow
********************************************/
void bubbleSort(char* movies_array_title[], double movies_array_rate[], int movies_array_yr[], int size)
{
   int i, j;
   bool swapped;
   for (i = 0; i < size-1; i++)
   {
     swapped = false;
     for (j = 0; j < size-i-1; j++)
     {
        if (movies_array_yr[j] > movies_array_yr[j+1])
        {
         //swap values with respect to the year
           swap_year(&movies_array_yr[j], &movies_array_yr[j+1]);
           swap_title(&movies_array_title[j], &movies_array_title[j+1]);
           swapped = true;
        }
     }
 
     // IF no two elements were swapped by inner loop, then break
     if (swapped == false)
        break;
   }
}


/******************************************
**                find_largest_file
** find the largest file and process them
**
********************************************/
void find_largest_file(const char* name){
  DIR* dir = opendir(name);                 //opens the current directory 

  char path[100];
  struct stat stats;
  int file_size;
  int current_size = 0;

  if(dir == NULL){
    return;
  }

  struct dirent* entity;                    //dirent could be a file or a directory 
  entity = readdir(dir);                    //read something to de-reference

  while(entity != NULL){
    
    //strncmp is used to compare the C string up to num characters
    //if they are equal until character differs or until num characters match in both strings 
    //whatever happens first and the length of movies string in this case is important
   if(strncmp("movies_", entity->d_name,strlen("movies_")) == 0 && strstr(entity->d_name, ".csv") != NULL ){
      stat(entity->d_name,&stats);            //set the name to stat address in order to get the file size 
      file_size = stats.st_size;

      //if the file size is bigger than the current size then replace it with the bigger one
      if(file_size > current_size){
          current_size = file_size;          //set the new file size as the largest size
          strcpy(path, entity->d_name);      //add the file name to a character array 
      }
    }


  entity = readdir(dir);                    //read something to de-reference to look through all of the files in the directory

  }
  closedir(dir);                            //close the directory 
  printf("Now processing the chosen file named: %s\n",path );

struct movie *films = process_file(path);  //process the largest file by parsing it


//making new directory
char directory_name[100];                  //holding the final directory name
srand(time(NULL));                         //intializatizing the seed, should only be called once
int random_num = random() % 100000;        //choose a random integer value between 0-99,999
sprintf(directory_name, "tonb.movies.%d" ,random_num); // story the name including the random number to the directory variable
mkdir(directory_name, 0750);               //make the directory with the final name 
printf("Created a directory with name %s\n", directory_name); //print the directory name 
create_yearly_movies(films, counter, directory_name); //create the 


}

void find_smallest_file(const char* name){
  DIR* dir = opendir(name);                 //opens the current directory 

  char path[100];
  struct stat stats;                        //properties for the file size 
  int file_size;
  int current_size = MAX_SIZE;

  if(dir == NULL){
    return;
  }

  struct dirent* entity;                    //dirent could be a file or a directory 
  entity = readdir(dir);                    //read something to de-reference

  while(entity != NULL){

    //strncmp is used to compare the C string up to num characters
    //if they are equal until character differs or until num characters match in both strings 
    //whatever happens first and the length of movies string in this case is important
   if(strncmp("movies_", entity->d_name,strlen("movies_")) == 0 && strstr(entity->d_name, ".csv") != NULL ){
      stat(entity->d_name,&stats);            //set the name to stat address in order to get the file size 
      file_size = stats.st_size;
      //if the file size is bigger than the current size then replace it with the bigger one
      if(file_size < current_size){
          current_size = file_size;          //set the new file size as the smallest size
          strcpy(path, entity->d_name);      //add the file name to a character array 
      }
    }


  entity = readdir(dir);                    //read something to de-reference to look through all of the files in the directory

  }
  closedir(dir);
  printf("Now processing the chosen file named: %s\n",path );

struct movie *films = process_file(path); //process the largest file by parsing it

//making new directory
char directory_name[100];
srand(time(NULL));                                     //intializatizing the seed, should only be called once
int random_num = random() % 100000;                    //choose a random integer value between 0-99,999
sprintf(directory_name, "tonb.movies.%d" ,random_num); // story the name including the random number to the directory variable
mkdir(directory_name, 0750);//make the directory with the final name 
printf("Created a directory with name %s\n", directory_name);

create_yearly_movies(films, counter, directory_name);

}

int find_specific_file(const char* name){
  DIR* dir = opendir(name);                 //opens the current directory 
  char str_response[50];
  char path[100];
  struct stat stats;
  int file_size;
  int current_size = MAX_SIZE;

  if(dir == NULL){
    return -1;
  }

  printf("Enter the complete file name\n");
  scanf("%s",&str_response);

  if( access( str_response, F_OK ) == 0 ) {
     
  }
  else{
    printf("\n The file %s was not found. Try again\n", str_response);
    return -1;
  }

  struct dirent* entity;                    //dirent could be a file or a directory 
  entity = readdir(dir);                    //read something to de-reference

  while(entity != NULL){
    
    //strncmp is used to compare the C string up to num characters
    //if they are equal until character differs or until num characters match in both strings 
    //whatever happens first and the length of movies string in this case is important
   if(strcmp(str_response, entity->d_name) == 0  ){
      stat(entity->d_name,&stats);            //set the name to stat address in order to get the file size 
          strcpy(path, entity->d_name);      //add the file name to a character array 

    }


  entity = readdir(dir);                    //read something to de-reference to look through all of the files in the directory

  }
  closedir(dir);
  printf("Now processing the chosen file named: %s\n",path );

struct movie *films = process_file(path); //process the largest file by parsing it


//making new directory
char directory_name[100];
srand(time(NULL));                         //intialization, should only be called once
int random_num = random() % 100000;
//itoa(random_num,directory_name, 10);
sprintf(directory_name, "tonb.movies.%d" ,random_num);

mkdir(directory_name, 0750);

printf("Created a directory with name %s\n", directory_name);
//printList(films);
//printf("\n counter:%d\n",counter);
create_yearly_movies(films, counter, directory_name);
return 1;
}


void create_yearly_movies(struct movie* node, int num_movies, char* directory_name){
 int movies_array_yr[num_movies];
 double movies_array_rate[num_movies];
 char * movies_array_title[num_movies];

   int i = 0;
//populatiing the array
   while(node != NULL){
   //printf("%s\n",node->title);
      movies_array_yr[i] = node->year;
      movies_array_rate[i] = node->rating;
      movies_array_title[i] = malloc(100);
      strcpy(movies_array_title[i], node->title);
       i++;
      node = node->next;
   }
//sort the years in order

bubbleSort(movies_array_title, movies_array_rate, movies_array_yr, num_movies);


int current_year = movies_array_yr[0];
char* current_title = movies_array_title[0];

int years[num_movies];
int count = 0;

for(int i = 1; i <= num_movies; i ++){


   //if it is a new year then print the current year highest rating
      if(current_year != movies_array_yr[i]  ){

         years[count] = current_year; //populating the existing years with no repeats
         current_year = movies_array_yr[i]; //new year
          count++;
      } 

}
FILE *f;
int size = sizeof(years)/sizeof(years[0]);
int num_opens = 0;
char file_name[50];
char temp[15];

current_year = years[0];
//creating file
for(i = 0; i < size; i++){

  for(int j = 0; j < num_movies; j++){

      if(years[i] == movies_array_yr[j]){

  
        strcpy(file_name, directory_name);
        //might need to create another char variable to append
        strcat(file_name, "/");
        sprintf(temp, "%d" ,years[i]);
        strcat(file_name,temp);
        strcat(file_name, ".txt");
  


        if( access( file_name, F_OK ) != 0 ) {
          current_year = years[i];
          open(file_name, O_RDWR | O_CREAT | O_TRUNC, 0640);
          f = fopen(file_name, "w");
        }         


        fprintf(f, "%s\n",movies_array_title[j] );
          
          if( access( file_name, F_OK ) != 0 ) {fclose(f);}
  
      }

  }
}

  for(int i = 0; i < num_movies; i++){
    free(movies_array_title[i] = NULL);
  }

}

//
struct movie* process_file(char *file){

  FILE * file_info;
   char const * filename = file;
   char lines[1024];
   counter = 0;

   if((file_info = fopen( filename, "r")) == NULL){
      printf("The file was unable to be opened and processed \n" );
      return NULL;
   }

fgets( lines, sizeof(lines), file_info); //gets rid of the first line

char *movie_token;
char *savepointer;


//link list variables
struct movie *head = NULL;
struct movie *tail = NULL;
struct movie *films = NULL;

while(fgets( lines, sizeof( lines), file_info) ){

//printf("LINES::::::%s", lines);
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
   tail->next = films;       // link the nodes together
   tail = tail->next;        // attach the tail of the linked list to the next node
   counter++;
}
}
tail->next = NULL; //attach the last node to NULL

//print out the amount of movies being parsed and the filename
printf("%d movies have been parsed from %s file \n\n",counter, filename); 

return head;
}




void print_array(char* movies_array_title[], double movies_array_rate[], int movies_array_yr[], int size){
   int i;
   for (i=0; i < size; i++)
   printf("%d; %s; %.1f \n",movies_array_yr[i], movies_array_title[i],  movies_array_rate[i]);
}

int choices_prompt(void){
   int response;

//prompts user 4 choices
   printf("\n What would you like to do?\n");
   printf("1. Show movies released in the specified year \n");
   printf("2. Show highest rated movie for each year \n");
   printf("3. Show the title and year of release of all movies in a specific language\n");
   printf("4. Exit from the program\n");
   scanf("%d", &response); // take in an integer response from user


   return response; //return response

};



struct movie* add_movie(char* lines){

//character pointers for reentrant function
char *movie_token;
char *savepointer;
char *languagepointer;

struct movie* films = malloc(sizeof(struct movie));

   movie_token = strtok_r(lines, ",", &savepointer); //obtaining each line of file and storing it in a character array
   films->title = malloc(strlen(movie_token) + 1);
   strcpy(films->title,movie_token);
   
   //year  
   movie_token = strtok_r(NULL, ",", &savepointer); //clear token
   films->year = atoi(movie_token);

   //languages
   movie_token = strtok_r(NULL, ",", &savepointer);

//parsing through the languages for each movie
//stack overflow referenced
char* temp;
int i = 0;
   for (temp = strtok_r(movie_token, "[;]", &languagepointer);
       temp != NULL;
       temp = strtok_r(NULL, "[;]", &languagepointer)){

      films->languages[i] = malloc(20); //assuming maximum length is 20
      strcpy(films->languages[i], temp);
      i++;
   }
   //need to fill the rest of the languages with NULL if there is none
   for(int j = i; j< 5; j++){films->languages[j]=NULL;}

   //ratings
   movie_token = strtok_r(NULL, ",", &savepointer);
   films->rating = strtod(movie_token, &savepointer);

 //  free(films->title);
   return films;

};