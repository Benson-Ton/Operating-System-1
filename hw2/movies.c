#include "movies.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

//int counter = 0;

void swap_title(char ** title_1, char ** title_2){
   char * temp = *title_1;
   *title_1 = *title_2;
   *title_2 = temp;
};

void swap_year(int* year_1, int* year_2){
   int temp = *year_1;
   *year_1 = *year_2;
   *year_2 = temp;
};

void swap_rating(double * rating_1, double * rating_2){
   double temp = *rating_1;
   *rating_1 = *rating_2;
   *rating_2 = temp;
};


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
           swap_rating(&movies_array_rate[j], &movies_array_rate[j+1]);
           swap_title(&movies_array_title[j], &movies_array_title[j+1]);
           swapped = true;
        }
     }
 
     // IF no two elements were swapped by inner loop, then break
     if (swapped == false)
        break;
   }
};


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
    //printf("%s\n",entity->d_name);
    
    //strncmp is used to compare the C string up to num characters
    //if they are equal until character differs or until num characters match in both strings 
    //whatever happens first and the length of movies string in this case is important
   if(strncmp("movies_", entity->d_name,strlen("movies_")) == 0 && strstr(entity->d_name, ".csv") != NULL ){
      stat(entity->d_name,&stats);            //set the name to stat address in order to get the file size 
      file_size = stats.st_size;
      //if the file size is bigger than the current size then replace it with the bigger one
      if(file_size > current_size){
          current_size = file_size;
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
srand(time(NULL));                         //intialization, should only be called once
int random_num = random() % 100000;
//itoa(random_num,directory_name, 10);
sprintf(directory_name, "tonb.movies.%d" ,random_num);
printf("%s\n", directory_name);
//mkdir(directory_name, 0750);


//need to loop through the years for the .txt name and add the movies 
// dir = opendir(directory_name);
// entity = readdir(dir);  
// printf("%s\n",entity->d_name);



//films = movie_year_list(films);

//int year_releases[];

//printf("%d",sizeof(year_releases));

//open(films->year, O_RDWR | O_CREAT | O_TRUNC, 0750);


//look into freeing memory by looping through each node of the linked list
free(films->title);
free(films);

  //closedir(dir);
}


//
struct movie* process_file(char *file){

  FILE * file_info;
   char const * filename = file;
   char lines[1024];
  int counter = 0;

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
//free(lines);
free(films->title);
free(films);
free(tail->next);
fclose(file_info);
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


struct movie* movie_year_list(struct movie* node){
   //int response;
   int count = 0;
   
   //printf("Please enter the year for which you want to see the movie \n");
   //scanf("%d", &response);

   while(node != NULL){
    //look through the link list and find the match title for the specified year
     if(node->year != 0){
      //printf("%s \n",node->title);
      return node;
      count++;
     }
      node = node->next;
   }
   if(count == 0){
    //  printf("There are no movies released in the year of %d \n\n", response);
    return NULL;
   }

};

void movie_rating_list(struct movie* node, int num_movies){
 int movies_array_yr[num_movies];
 double movies_array_rate[num_movies];
 char * movies_array_title[num_movies];

   int i = 0;
//populatiing the array
   while(node != NULL){
      //---
         //need to compare the ratings within the same year

         //if(rating > rating){printf("%.1f", node->rating);}
      movies_array_yr[i] = node->year;
      movies_array_rate[i] = node->rating;
      movies_array_title[i] = malloc(100);
      strcpy(movies_array_title[i], node->title);
       i++;
      node = node->next;
   }

bubbleSort(movies_array_title, movies_array_rate, movies_array_yr, num_movies);


double highest_rating = movies_array_rate[0];
int current_year = movies_array_yr[0];
char* current_title = movies_array_title[0];


for(int i = 1; i < num_movies; i ++){

   //if it is a new year then print the current year highest rating
      if(current_year != movies_array_yr[i] || i == num_movies-1){
         printf("%d %.1f %s \n", current_year, highest_rating, current_title);
         highest_rating = movies_array_rate[i];
         current_year = movies_array_yr[i];
         current_title = movies_array_title[i];
        
      }
      //set the new high rating if the following movie rating is higher
         if(highest_rating < movies_array_rate[i]){
            highest_rating = movies_array_rate[i];
            current_year = movies_array_yr[i];
            current_title = movies_array_title[i];
         }
}

// free(movies_array_title);
for(int i = 0; i < num_movies; i++){
   free(movies_array_title[i] = NULL);
}
}


void movie_language_list(struct movie* node){
   char  response[20];
   int   condition = -1;
   int   count = 0;

   printf("Please enter the language for which you want to see movies (Case Sensitive) \n");
  // fgets(response,20,stdin);
   scanf("%s", &response);


   while(node != NULL){
      for(int i = 0; i < 5; i++){
         if(node->languages[i] != NULL){
            condition = strcmp(node->languages[i], response); // compare the response and available language
           //print the languages if there is a match
            if(condition == 0){
                   printf("%d %s\n",node->year, node->title);
                   count++;
            }
         }
      }
   node = node->next; //look at the next movie
   }
   //if there are no movies with the specific language then print it
   if(count == 0){printf("\nThere are no movies released in %s \n", response);}

}



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