#ifndef movies_h
#define movies_h

struct movie{

//information about the movie
	char* title;
	int year;
	char* languages[5];
	double rating; 

	struct movie* next;

};
struct movie* process_file(char * file );

void find_largest_file(const char* name);



int choices_prompt(void);

void swap_title(char ** title_1, char ** title_2);
void swap_year(int* year_1, int* year_2);
void swap_rating(double * rating_1, double * rating_2);

void bubbleSort(char* movies_array_title[], double movies_array_rate[], int movies_array_yr[], int size);

struct movie* movie_year_list(struct movie* node);
void movie_rating_list(struct movie* node, int num_movies);
void movie_language_list(struct movie* node);


struct movie* add_movie(char* lines);



#endif
