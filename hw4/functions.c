#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include "functions.h"

//cited: https://www.geeksforgeeks.org/c-program-replace-word-text-another-given-word/
//this function is used to replace the ++ with a carrot. While doing this it will resize the buffer array
char* replaceplus(const char* buf, const char* plus, const char* replacewith)
{
    char* result;
    int i, count = 0;
    int plus_len = strlen(replacewith);
    int replacewith_len = strlen(plus);
  
    // need to count how many characters are in the string
    // check whether it exists in the string
    for (i = 0; buf[i] != '\0'; i++) {
        if (strstr(&buf[i], plus) == &buf[i]) {
            count++;
  
            // Jumping to index after the old word.
            i += replacewith_len - 1;
        }
    }
  
    // Making new string of enough length
    result = (char*)malloc(i + count * (plus_len - replacewith_len) + 1);
  
    i = 0;
    while (*buf) {
        // compare the substring with the buffer
        if (strstr(buf, plus) == buf) {
            strcpy(&result[i], replacewith);
            i += plus_len;
            buf += replacewith_len;
        }
        else
            result[i++] = *buf++;
    }
  
    result[i] = '\0'; //end it with the null character
    return result;
}

/*


		

		getline(&text_buf,&size, stdin);
	//	index++;
		printf("%s\n", text_buf);
		printf("%d\n", strlen(text_buf));
		for (int i = 0; i < strlen(text_buf); ++i)
		{
			printf("test\n");
			if(text_buf[i] < 32 || text_buf[i] > 126){return EXIT_FAILURE;}
		}



*/