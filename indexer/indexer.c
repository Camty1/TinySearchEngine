/* indexer.c --- 
 * 
 * 
 * Author: Raif T. Olson
 * Created: Fri Feb 10 12:55:31 2023 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <hash.h>
#include <webpage.h>
#include <pageio.h>


#define HASH_SIZE 100 

int total_word_count = 0;

typedef struct {
    char* word;
    int count;
} wordCount_t;

bool wordMatch(void *elementp, const void* keyp);
void removeWords(void* elementp);
void calculate_total(void* elementp);
void print_element(void* elementp);

static int NormalizeWord(char *word, int word_len) {
	if (word_len > 2) {
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
	}
	else {
		return -1;
	}
	return 0;
}

int main(void) {
	webpage_t *webpage_1 = pageload(1, "../pages");
	int pos = 0;
	char *word;

    FILE* output;

    output = fopen("indexerOut", "w");
		/*
	while ((pos = webpage_getNextWord(webpage_1, pos, &word)) > 0) {
        int res = NormalizeWord(word, strlen(word));
		if (res == 0) 
            fprintf(output, "%s\n", word);
		
        free(word);
	}

    fclose(output);
		*/
    hashtable_t* index = hopen(HASH_SIZE);
    //pos = 0;
    while ((pos = webpage_getNextWord(webpage_1, pos, &word)) > 0) {
        int res = NormalizeWord(word, strlen(word));
        if (res == 0) {
		    wordCount_t* result = (wordCount_t*) hsearch(index, wordMatch, word, strlen(word)); 
			// if word already in hash table, increment its count by 1
			if (result != NULL) {
				result -> count = result -> count + 1;
                free(word);
			}
			// if word not in hash table, add it to hash table with count 1
			else {
			    wordCount_t* newWord = (wordCount_t*)malloc(sizeof(wordCount_t));
			    newWord -> word = word;
			    newWord -> count = 1;
			    int hashRes = hput(index, newWord, word, strlen(word));
			    if (hashRes != 0)
			    	return -1;
			}
        }
        else {
            free(word);
        }
	}
	fclose(output);
    happly(index, print_element);

	happly(index, calculate_total);
	printf("total words: %d\n", total_word_count);
	happly(index, removeWords);
	hclose(index);
	webpage_delete(webpage_1);
	return 0;
}

void removeWords(void *elementp) {
    wordCount_t* wordCount = (wordCount_t*) elementp;
    free(wordCount->word);
	free(wordCount);
}

bool wordMatch(void* elementp, const void* keyp) {
	wordCount_t* entry = (wordCount_t*) elementp;
	const char* key = (char*) keyp;

	char* entry_word = entry -> word;

	return (strcmp(entry_word, key) == 0);
}


void calculate_total(void* elementp) {
	wordCount_t* entry = (wordCount_t*) elementp;
	total_word_count = total_word_count + entry -> count;
}

void print_element(void* elementp) {
    wordCount_t* entry = (wordCount_t*) elementp;
    printf("%s-%d\n", entry->word, entry->count);
}
