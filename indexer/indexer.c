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


#define HASH_SIZE 100 

int total_word_count = 0;

typedef struct {
    char* word;
    int count;
} wordCount_t;

typedef struct {
    char* word;
    queue_t* documentQueue;
} wordQueue_t;

typedef struct {
    int documentId;
    int count;
} docWordCount_t;

static bool wordMatch(void *elementp, const void* keyp);
static bool wordQueueMatch(void* elementp, const void* keyp);
static void removeWords(void* elementp);
static void calculate_total(void* elementp);
static void printElement(void* elementp);
static int normalizeWord(char* word, int word_len);
static bool matchDocumentIds(void* elementp, const void* keyp);
static void removeDocCount(void *elementp);
static void removeWordQueue(void *elementp);
static void sumWords(void* elementp);
static void sumQueueWords(void* elementp);
static void indexPage(hashtable_t* index, int document, char* dirName);
static void printQueueElement(void* elementp);

int main(int argc, char* argv[]) {

    if (argc == 2) {
        // Parse terminal input
        char* extra;
        int document = strtol(argv[1], &extra, 10);
        
        // Create index
        hashtable_t* index = hopen(HASH_SIZE);

        // Loop through documents
        for (int i = 1; i <= document; i++) {
            indexPage(index, i, "../pages");
        }

        // Get total number of words
        happly(index, sumWords);
        printf("%d\n", total_word_count);

        // Memory management
        happly(index, removeWordQueue);
        hclose(index);
    }
    
    // OLD STUFF -- KEEPING HERE JUST IN CASE

    //happly(index, printElement);
    //FILE* output;

    //output = fopen("indexerOut", "w");
		/*
	while ((pos = webpage_getNextWord(webpage, pos, &word)) > 0) {
        int res = NormalizeWord(word, strlen(word));
		if (res == 0) 
            fprintf(output, "%s\n", word);
		
        free(word);
	}

    fclose(output);
		*/
    /*
    while ((pos = webpage_getNextWord(webpage, pos, &word)) > 0) {
        int res = normalizeWord(word, strlen(word));
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
    */
    /*
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
                    qput(hashSearch, newCount);
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
    */
    //happly(index, print_element);
    //fclose(output);
	return 0;
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

static void removeWords(void *elementp) {
    wordCount_t* wordCount = (wordCount_t*) elementp;
    free(wordCount->word);
	free(wordCount);
}

static void removeWordQueue(void *elementp) {
    wordQueue_t* wq = (wordQueue_t*) elementp;
    free(wq->word);
    qapply(wq->documentQueue, removeDocCount);
    qclose(wq->documentQueue);
    free(wq);
}

static void removeDocCount(void *elementp) {
    free(elementp);
}


static bool wordMatch(void* elementp, const void* keyp) {
	wordCount_t* entry = (wordCount_t*) elementp;
	const char* key = (char*) keyp;

	char* entry_word = entry -> word;

	return (strcmp(entry_word, key) == 0);
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

static void calculate_total(void* elementp) {
	wordCount_t* entry = (wordCount_t*) elementp;
	total_word_count = total_word_count + entry -> count;
}

static void sumWords(void* elementp) {
    wordQueue_t* entry = (wordQueue_t*) elementp;
    qapply(entry->documentQueue, sumQueueWords);
}

static void sumQueueWords(void* elementp) {
    docWordCount_t* doc = (docWordCount_t*) elementp;
    total_word_count += doc -> count;
}

static void printElement(void* elementp) {
    wordQueue_t* entry = (wordQueue_t*) elementp;
    printf("%s", entry->word);
    qapply(entry->documentQueue, printQueueElement);
    printf("\n");
}

static void printQueueElement(void* elementp) {
    docWordCount_t* doc = (docWordCount_t*) elementp;
    printf("-%d,%d", doc->documentId, doc->count);
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
