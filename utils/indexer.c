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
#include <queue.h>
#include <hash.h>
#include <webpage.h>
#include <pageio.h>
#include <indexio.h>
#include <indexer.h>

#define HASH_SIZE 100 

hashtable_t* index_all_pages(char* dirnm, char* indexnm);
void closeIndex(hashtable_t* index);
static int normalizeWord(char *word, int word_len);
static bool matchDocumentIds(void* elementp, const void* keyp);
static bool wordQueueMatch(void* elementp, const void* keyp);
void removeDocCount(void *elementp);
static void removeWordQueue(void *elementp);
static void indexPage(hashtable_t* index, int document, char* dirName);

hashtable_t* index_all_pages(char* dirnm, char* indexnm) {
			char filepath[100];
			FILE* file;
			
			int i = 1;
			sprintf(filepath, "%s/%d", dirnm, i);

			hashtable_t* index = hopen(HASH_SIZE);
			bool read_file = false;
			// Parse terminal input
			// Loop through documents
			while ((file = fopen(filepath, "r"))) {
				read_file = true;
				fclose(file);
				indexPage(index, i, dirnm);
				i++;
				sprintf(filepath, "%s/%d", dirnm, i);
			}
			if (read_file) {
				indexSave(index, indexnm);
				return index;
			}
			else
				return NULL;
			
}

void closeIndex(hashtable_t* index) {
	happly(index, removeWordQueue);
	hclose(index);
}

static void indexPage(hashtable_t* index, int document, char* dirName) {
	int pos = 0;
	char *word;
	
	// Load in webpage 
	webpage_t *webpage = pageload(document, dirName);
	
	// Go through every word on webpage 
	while ((pos = webpage_getNextWord(webpage, pos, &word)) > 0) {
		int res = normalizeWord(word, strlen(word));
		if (res == 0) {
			wordQueue_t* hashSearch = hsearch(index, wordQueueMatch, word, strlen(word));
			
			// Word isn't in index yet
			if (hashSearch == NULL) {
				
				// Create a word queue and initialize it
				wordQueue_t* docQueue = (wordQueue_t*) malloc(sizeof(wordQueue_t));
				docQueue->word = word;
				queue_t* queue = qopen();
				docQueue->documentQueue = queue;
				
                // Put document in queue with word count 1
				docWordCount_t* newCount = malloc(sizeof(docWordCount_t));
				newCount->documentId = document;
				newCount->count = 1;
				res = qput(queue, newCount);
                hput(index, docQueue, word, strlen(word));
			}
			
			// Word is in index
			else {
				docWordCount_t* queueSearch = (docWordCount_t*) qsearch(hashSearch->documentQueue, matchDocumentIds, &document); 
				// Document isn't in queue
				if (queueSearch == NULL) {
					docWordCount_t* newCount = malloc(sizeof(docWordCount_t));
					newCount->documentId = document;
					newCount->count = 1;
					qput(hashSearch->documentQueue, newCount);
				}
				// Document is in queue
				else {
					queueSearch->count++;
				}
				free(word);
			}
        }
		// Word is malformed or too short, get it out of here
		else {
			free(word);
		}
	}
	webpage_delete(webpage);
}


static void removeWordQueue(void *elementp) {
    wordQueue_t* wq = (wordQueue_t*) elementp;
    free(wq->word);
    qapply(wq->documentQueue, removeDocCount);
    qclose(wq->documentQueue);
    free(wq);
}

void removeDocCount(void *elementp) {
    free(elementp);
}

static bool wordQueueMatch(void* elementp, const void* keyp) {
	wordQueue_t* entry = (wordQueue_t*) elementp;
	const char* key = (char*) keyp;

	char* entry_word = entry -> word;

	return (strcmp(entry_word, key) == 0);
}

static bool matchDocumentIds(void* elementp, const void* keyp) {
    int* pointer = (int*) keyp;
    docWordCount_t* element = (docWordCount_t*) elementp;

    return (*pointer == element->documentId);
}

static int normalizeWord(char *word, int word_len) {
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
