/* query.c --- 
 * 
 * 
 * Author: Raif T. Olson
 * Created: Tue Feb 21 10:52:01 2023 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */


#include <stdio.h>                                                            
#include <stdlib.h>                                                           
#include <stdint.h>                                                           
#include <stdbool.h>
#include <string.h>                                                           
#include <ctype.h>                                                            
#include <queue.h>                                                            
#include <hash.h>                                                             
#include <webpage.h>                                                          
#include <pageio.h>                                                           
#include <indexio.h>

#define MAX_QUERY 200

static void printQueryWord(void* elementp);
static int normalizeWord(char *word, int word_len);

/* if (strcmp(query_word, "^D")) {
	running = false;
	break;
 } else 
	
*/

int main(int argc, char* argv[]) {
	bool running = true;
	while (running) {
		char search_query[MAX_QUERY];
		char* query_word;
		bool query_valid = true;
		printf("> ");
		scanf("%[^\n]s", search_query);


		query_word = strtok(search_query, " ");
		queue_t* query_q = qopen();
		while (query_word != NULL) {
			if (normalizeWord(query_word, strlen(query_word)) == 0) {
				qput(query_q, query_word);
			} else {
				query_valid = false;
				break;
			}
			query_word = strtok(NULL, " ");
		}
		if (query_valid) {
			qapply(query_q, printQueryWord);
			printf("\n");
		} else {
			printf("[invalid query]\n");
		}
		// flushes the standard input to allow for new query to be entered
		int c;
		while ((c = getchar()) != '\n') {
			if (c == EOF) {
				running = false;
				break;
			}
		}
		
		
	}
	exit(EXIT_SUCCESS);
}


static int normalizeWord(char *word, int word_len) {                          
	for (int i=0; i<word_len; i++) {                                           
		char letter = word[i];                                                   
		int res = isalpha(letter);                                               
		if (res != 0) {                                                          
			if ((64 < letter) && (letter < 91)) {                                  
				word[i] = tolower(letter);                                           
			}                                                                     
		} else {                                                                 
			return -1;                                                             
		}                                                                        
	}
  
	return 0;                                                                    
}                                                                              

static void printQueryWord(void* elementp) {
	char* query_word = (char*) elementp;
	printf("%s ", query_word);
}
