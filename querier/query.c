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
#define MAX_DOCS 1000

static void printQueryWord(void* elementp);
static int normalizeWord(char *word, int word_len);
static int rankDocument(queue_t* queries, hashtable_t* index, int docId);
static int getOccurence(hashtable_t* index, char* word, int docId);
static bool compareId(void* elementp, const void* keyp);
static bool compareWord(void* elementp, const void* keyp);
static void getDocIDs(void* elementp);
static void idFromQueue(void* elementp);
static bool matchingIDs(void* elementp, const void* keyp);
static void printDocs(void* elementp);
static int rankNDocuments(queue_t* query_q, hashtable_t* index);

queue_t* docIDs;


typedef struct {
    char* word;
    queue_t* documentQueue;
} wordQueue_t;

typedef struct {
    int documentId;
    int count;
} docWordCount_t;


static void getDocIDs(void* elementp) {
	wordQueue_t* wordQueue = (wordQueue_t*)elementp;
	char* word = wordQueue->word;
	//	printf("%s\n", word);
	queue_t* docQueue = wordQueue->documentQueue;
	qapply(docQueue, idFromQueue);
}

static void idFromQueue(void* elementp) {
	docWordCount_t* entry = (docWordCount_t*)elementp;
	int docId = entry->documentId;
	int* docIDp = (int*)malloc(sizeof(int*));
	docIDp = &docId;
	//*docIDp = docId;
	int* result = (int*)qsearch(docIDs, matchingIDs, docIDp);
		if (result == NULL) {
		qput(docIDs, docIDp);
		
	}
	
	//qapply(docIDs, printDocs);
	//	printf("\n\n");
	
}

static bool matchingIDs(void* elementp, const void* keyp) {
	int* queueID = (int*)elementp;
	int* val = (int*)keyp;
	return (*queueID == *val);
}

static void printDocs(void* elementp) {
	int* doc = (int*)elementp;
	printf("doc id: %d\n", *doc);
}

	
int main(int argc, char* argv[]) {
    // Load in index
    hashtable_t* index = indexLoad("../indexer/step2");
		/*docIDs = qopen();
		qapply(docIDs, printDocs);
		happly(index, getDocIDs);
		qapply(docIDs, printDocs);*/
		
	bool running = true;
	
    // Query prompt loop
    while (running) {
		char search_query[MAX_QUERY];
		char* query_word;
		bool query_valid = true;
		printf("> ");
        // Get string from console
		if (scanf("%[^\n]s", search_query) != EOF) {
			
            // Open query queue
            queue_t* query_q = qopen();
            
            // Go through words in query
			query_word = strtok(search_query, " ");
			while (query_word != NULL) {

                // Make sure word is valid
				if (normalizeWord(query_word, strlen(query_word)) == 0) {
                    // Put word in query queue
					qput(query_q, query_word);
				} 

                else {
					query_valid = false;
					break;
				}
				query_word = strtok(NULL, " ");
			}
            
            // Query is valid
			if (query_valid) {
				qapply(query_q, printQueryWord);
				printf("\n");
			  int i = rankNDocuments(query_q, index);
				//              int rank = rankDocument(query_q, index, 1); // Step 2
			}

            else {
				printf("[invalid query]\n");
			}
			// flushes the standard input to allow for new query to be entered
			int c;
			while ((c = getchar()) != '\n');
			
		}
		else {
			printf("\n");
			running = false;
		}
	}
		
	exit(EXIT_SUCCESS);
}

static int rankNDocuments(queue_t* query_q, hashtable_t* index) {

	queue_t* copy_query_q = qopen();
	queue_t* copy_doc_q = qopen();
	bool is_first_word = true;
	queue_t* intersection = qopen();
	char* word;
	docWordCount_t* doc;
	int* doc_id;
	
	while ((word = (char*) qget(query_q)) != NULL) {
		qput(copy_query_q, word);

		wordQueue_t* word_q = hsearch(index, compareWord, word, strlen(word));
		queue_t* doc_q = word_q->documentQueue;

		if (is_first_word) {
			
			while((doc = (docWordCount_t*)qget(doc_q)) != NULL) {
				qput(copy_doc_q, doc);
				
				int* doc_id = (int*)malloc(sizeof(int*));
				*doc_id = doc->documentId;
				qput(intersection, doc_id);
			}
			is_first_word = false;
		}
		
		else {
			queue_t* copy_intersection = qopen();
			while ((doc_id = (int*)qget(intersection)) != NULL) {
				
				docWordCount_t* search_result = qsearch(doc_q, compareId, doc_id);
				if (search_result != NULL) {
					qput(copy_intersection, doc_id);
				}
			}
			qconcat(intersection, copy_intersection);
		}
	}

	qapply(intersection, printDocs);
	
	return 0;
		
}

static int normalizeWord(char *word, int word_len) {

	for (int i=0; i<word_len; i++) {
		char letter = word[i];
		int res = isalpha(letter);
	
        if (res != 0) {
		
            if ((64 < letter) && (letter < 91)) {
				word[i] = tolower(letter);
			}
		}
        
        else {
			return -1;
		}
	}
    
	return 0;
}

static void printQueryWord(void* elementp) {
	char* query_word = (char*) elementp;
	printf("%s ", query_word);
}

static int rankDocument(queue_t* queries, hashtable_t* index, int docId) {
    // Variable initialization
    char* word;
    int rank = INT_MAX;

    // Go through words in query queue
    while ((word = (char*) qget(queries)) != NULL) {

        // These words should be ignored
        if (strlen(word) < 3 || strcmp(word, "and") == 0 || strcmp(word, "or") == 0) {
            
        }

        else {
            // Get number of occurences in index for a given document
            int occurence = getOccurence(index, word, docId);
            printf("%s:%d ", word, occurence);

            // Tracking minimum occurence for rank
            if (occurence < rank) {
                rank = occurence;
            }
        }
    }
    // At least one word was valid and was given an occurence
    if (rank != INT_MAX) {
        printf("-- %d\n", rank);
        return rank;
    }
    // Either query queue was empty or no valid words
    else {
        printf("-- 0\n");
        return 0;
    }
}

static int getOccurence(hashtable_t* index, char* word, int docId) {

    // Variable initialization
    int occurence = 0;

    // Search for word in index
    wordQueue_t* indexElement = (wordQueue_t*) hsearch(index, compareWord, word, strlen(word));

    // Word is in index
    if (indexElement != NULL) {

        // Search for document in wordQueue
        docWordCount_t* queueElement = (docWordCount_t*) qsearch(indexElement->documentQueue, compareId, (const void *) &docId);
        
        // Document is in queue
        if (queueElement != NULL) {
            occurence = queueElement->count;
        }

    }
    
    // Return number of occurences of word in given document
    return occurence;
}

// Compare words to word queues for hsearch
static bool compareWord(void* elementp, const void* keyp) {
    wordQueue_t* wq = (wordQueue_t*) elementp;
    char* key = (char*) keyp;
    return (strcmp(wq->word, key) == 0);
}

// Compare ints to document word counts document ids
static bool compareId(void* elementp, const void* keyp) {
    docWordCount_t* wc = (docWordCount_t*) elementp;
    int* id = (int*) keyp;
    return (wc->documentId == *id);
}
