#include "movies.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


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


void movie_year_list(struct movie* node){
   int response;
   int count = 0;
   
   printf("Please enter the year for which you want to see the movie \n");
   scanf("%d", &response);


   while(node != NULL){
    //look through the link list and find the match title for the specified year
     if(node->year == response){
      printf("%s \n",node->title);
      count++;
     }
      node = node->next;
   }
   if(count == 0){
      printf("There are no movies released in the year of %d \n\n", response);
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


int movie_language_list(struct movie* node){
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
   if(count == 0){
      printf("\nThere are no movies released in %s \n", response);
      return -1;
    }

return 0;
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


   return films;

};