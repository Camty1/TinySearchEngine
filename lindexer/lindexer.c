/* lindexer.c --- 
 * 
 * 
 * Author: Cameron J Wolfe and Hannah M Brookes
 * Created: Mon Mar 3 20:04:37 2023 (-0500)
 * Version: 1
 * 
 * Description: 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <lqueue.h>
#include <lhash.h>
#include <webpage.h>
#include <pageio.h>
#include <indexio.h>

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

typedef struct {
    lqueue_t* pageQueue;
    lhashtable_t* index;
    char* dirName;
} lindexer_args_t;

static bool wordQueueMatch(void* elementp, const void* keyp);
static int normalizeWord(char* word, int word_len);
static bool matchDocumentIds(void* elementp, const void* keyp);
static void removeDocCount(void *elementp);
static void removeWordQueue(void *elementp);
static void indexPage(hashtable_t* index, int document, char* dirName);
void* concurrentIndex(void* largs);
static void closeLindex(lhashtable_t* index);

int main(int argc, char* argv[]) {
	if (argc == 4) {
		char* dirnm = argv[1];
		char* indexnm = argv[2];
        
        char* endptr;
        uint32_t numThreads = strtoul(argv[3], &endptr, 10);

        if (endptr[0] == '-') {
            printf("numThreads must be a positive integer\n");
            exit(EXIT_FAILURE);
        }

        lqueue_t* pageQueue = lqopen();
        
        char filePath[100];
        FILE* file;
        int counter = 1;
        
        sprintf(filePath, "%s/%d", dirnm, counter);

        while((file = fopen(filePath, "r")) != NULL) {
            fclose(file);
            int* i = (int*) malloc(sizeof(int));
            *i = counter;
            lqput(pageQueue, i);
            counter++; 
            sprintf(filePath, "%s/%d", dirnm, counter);
        }
		lhashtable_t* index = lhopen(HASH_SIZE);
        
        lindexer_args_t* args = (lindexer_args_t*) calloc(1, sizeof(lindexer_args_t));
        args->pageQueue = pageQueue;
        args->index = index;
        args->dirName = dirnm;

        pthread_t* thread_list = calloc(numThreads, sizeof(pthread_t));

        for (int j = 0; j < numThreads; j++) {
            if (pthread_create(&thread_list[j], NULL, concurrentIndex, args) != 0) {
                printf("Failed to create thread\n");
                exit(EXIT_FAILURE);
            }
        }

        for (int k = 0; k < numThreads; k++) {
            if (pthread_join(thread_list[k], NULL) != 0) {
                printf("Failed join\n");
                exit(EXIT_FAILURE);
            }
        }
        
        free(thread_list);
        free(args);

        lindexSave(index, indexnm);

        lqclose(pageQueue);

        closeLindex(index);


	}
    else {
        printf("usage: lindexer <dirname> <indexnm> <numthreads>\n");
        exit(EXIT_FAILURE);
    }
  
	exit(EXIT_SUCCESS);
}


static void closeLindex(lhashtable_t* index) {
	lhapply(index, removeWordQueue);
	lhclose(index);
}


static void indexPage(lhashtable_t* index, int document, char* dirName) {
	int pos = 0;
	char *word;
	
	// Load in webpage 
	webpage_t *webpage = pageload(document, dirName);
	
	// Go through every word on webpage 
	while ((pos = webpage_getNextWord(webpage, pos, &word)) > 0) {
		int res = normalizeWord(word, strlen(word));
		if (res == 0) {
			wordQueue_t* hashSearch = lhsearch(index, wordQueueMatch, word, strlen(word));
		    
            lhlock(index);

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
                lhputsketch(index, docQueue, word, strlen(word));
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
            lhunlock(index);
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

static void removeDocCount(void *elementp) {
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

void* concurrentIndex(void* largs) {
    lindexer_args_t* args = (lindexer_args_t*) largs;
    lqueue_t* pageQueue = args->pageQueue;
    lhashtable_t* index = args->index;
    char* dirName = args->dirName;

    int* id;

    while ((id = lqget(pageQueue)) != NULL) {
        indexPage(index, *id, dirName);

        free(id);

    }

    return NULL;

}


