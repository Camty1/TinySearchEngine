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
static int rankDocument(queue_t* queries, hashtable_t* index, int docId, bool step2);
static int getOccurence(hashtable_t* index, char* word, int docId);
static bool compareId(void* elementp, const void* keyp);
static bool compareWord(void* elementp, const void* keyp);
static queue_t* documentIntersection(queue_t* query_q, hashtable_t* index);
static void printDocURL(int docID);
static void closeIndex(hashtable_t* index);
static void removeWordQueue(void *elementp);
static void removeDocCount(void *elementp);
static bool query_to_queue(queue_t* query_q, char* search_query);
static void printRanks(queue_t* query_q, hashtable_t* index, queue_t* intersection);

typedef struct {
    char* word;
    queue_t* documentQueue;
} wordQueue_t;

typedef struct {
    int documentId;
    int count;
} docWordCount_t;


int main(int argc, char* argv[]) {
    // Load in index
    hashtable_t* index = indexLoad("../indexer/testFile");

		// do tests for valgrind.sh
		if (argc == 2) {
			char sample_query[MAX_QUERY];
			strcpy(sample_query, "good coding");
			//strcpy(sample_query, "hello");
			queue_t* query_q = qopen();
			bool query_valid = query_to_queue(query_q, sample_query);
			// Query is valid
			if (query_valid) {
				qapply(query_q, printQueryWord);
				printf("\n");
				queue_t* intersection;
				intersection = documentIntersection(query_q, index);
				printRanks(query_q, index, intersection);
				qapply(intersection, removeDocCount);
				qclose(intersection);
			}
			else {
				printf("[invalid query]\n");
			}
			qclose(query_q);
		}
		else {
			bool running = true;
			// Query prompt loop
			while (running) {
				char search_query[MAX_QUERY];
				printf("> ");
				// Get string from console
				if (scanf("%[^\n]s", search_query) != EOF) {		
					// Open query queue
					queue_t* query_q = qopen();
					bool query_valid = query_to_queue(query_q, search_query);
					// Query is valid
					if (query_valid) {
						qapply(query_q, printQueryWord);
						printf("\n");
						queue_t* intersection;
						intersection = documentIntersection(query_q, index);
						printRanks(query_q, index, intersection);
						qapply(intersection, removeDocCount);
						qclose(intersection);
						//				int rank = rankDocument(query_q, index, 1, true); // Step 2
					}					
					else {
						printf("[invalid query]\n");
					}
					qclose(query_q);
					// flushes the standard input to allow for new query to be entered
					int c;
					while ((c = getchar()) != '\n');
				}
				// EOF character entered - end program
				else {
					printf("\n");
					running = false;
				}
			}
		}
		closeIndex(index);
		exit(EXIT_SUCCESS);
}


static void printRanks(queue_t* query_q, hashtable_t* index, queue_t* intersection) {
	int* first_doc_id = (int*)qget(intersection);
	if (first_doc_id == NULL) 
		printf("no matching documents.\n");
	else {
		int* doc_id;
		int rank;
		rank = rankDocument(query_q, index, *first_doc_id, false);
		printf("rank: %d, ", rank);
		printf("doc: %d, ", *first_doc_id);
		printDocURL(*first_doc_id);
		while ((doc_id = (int*) qget(intersection)) != NULL) {
			rank = rankDocument(query_q, index, *doc_id, false);
			printf("rank: %d, ", rank);
			printf("doc: %d, ", *doc_id);
			printDocURL(*doc_id);
		}
	}
}


static bool query_to_queue(queue_t* query_q, char* search_query) {
	// Go through words in query
	bool query_valid = true;
	char* query_word;
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
	return query_valid;
}


static void closeIndex(hashtable_t* index) {
	happly(index, removeWordQueue);
	hclose(index);
}

static void removeWordQueue(void *elementp) {
	wordQueue_t* wq = (wordQueue_t*)elementp;
	free(wq->word);
	qapply(wq->documentQueue, removeDocCount);
	qclose(wq->documentQueue);
	free(wq);
}

static void removeDocCount(void *elementp) {
	free(elementp);
}


static void printDocURL(int docID) {
	char buffer[100];
	snprintf(buffer, 100, "../pages/%d", docID);
	FILE* inputFile = fopen(buffer, "r");
	char url[1000];
	fgets(url, 1000, inputFile);
	fclose(inputFile);
	printf("%s", url);
}

static queue_t* documentIntersection(queue_t* query_q, hashtable_t* index) {
	// creates copies of queues to deal with removing items without
	// losing information
	queue_t* copy_query_q = qopen();
	queue_t* copy_doc_q = qopen();
	// different behavior if the word is the first in the query or not
	bool is_first_word = true;
	// queue to store the intersection of document IDs from all words in
	// the query
	queue_t* intersection = qopen();
	char* word;
	docWordCount_t* doc;
	int* doc_id;
	// gets words in query queue one by one (each time removes word)
	while ((word = (char*) qget(query_q)) != NULL) {
		if (strlen(word) > 2 && strcmp(word, "and") != 0 && strcmp(word, "or") != 0) {
			// put word in copy of queue to save for later
			qput(copy_query_q, word);
			// searches the index for the given word in query
			wordQueue_t* word_q = hsearch(index, compareWord, word, strlen(word));
			// if the word is not in the index, return NULL since no documents
			// exist that match query; otherwise, find all documents the word
			// is in
			if (word_q != NULL) {
				queue_t* doc_q = word_q->documentQueue;
				// if the word is first in query, find all documents that the
				// word is in and add them to the queue intersection
				if (is_first_word) {
					while((doc = (docWordCount_t*)qget(doc_q)) != NULL) {
						qput(copy_doc_q, doc);
						int* doc_id = (int*)malloc(sizeof(int*));
						*doc_id = doc->documentId;
						qput(intersection, doc_id);
					}
					// restores the document queue from the index and closes the
					// copy queue
					qconcat(doc_q, copy_doc_q);
					is_first_word = false;
				}
				// if word is not first in query, go through all documents
				// existing in the intersection queue and check whether the word
				// exists in each document; if it does, add to the queue
				// containing updated intersection, otherwise do nothing
				else {
				queue_t* copy_intersection = qopen();
				while ((doc_id = (int*)qget(intersection)) != NULL) {
					docWordCount_t* search_result = qsearch(doc_q, compareId, doc_id);
					if (search_result != NULL) {
						qput(copy_intersection, doc_id);
					}
					else {
						free(doc_id);
					}
				}
				// updates the queue intersection with the document IDs for
				// which all words in the query thus far exist in
				qconcat(intersection, copy_intersection);
				}
			}
			else {
				qclose(copy_query_q);
				qapply(copy_doc_q, removeDocCount);
				qclose(copy_doc_q);
				qapply(intersection, removeDocCount);
				qclose(intersection);
				return NULL;
			}
		}
	}
	// restores the query queue for use in rank document function
	qconcat(query_q, copy_query_q);
	return intersection;
		
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


static int rankDocument(queue_t* queries, hashtable_t* index, int docId, bool step2) {
    // Variable initialization
    char* word;
    int rank = INT_MAX;
		queue_t* copy_queries = qopen();
    // Go through words in query queue
    while ((word = (char*) qget(queries)) != NULL) {
			qput(copy_queries, word);
        // These words should be ignored
			/* if (strlen(word) < 3 || strcmp(word, "and") == 0 || strcmp(word, "or") == 0) {
				
				 } */
			
			//	else {
			// Get number of occurences in index for a given document
			int occurence = getOccurence(index, word, docId);
			if (step2)
				printf("%s:%d ", word, occurence);
			
			// Tracking minimum occurence for rank
			if (occurence < rank) {
				rank = occurence;
			}
		}
    
		
		qconcat(queries, copy_queries);
    // At least one word was valid and was given an occurence
    if (rank != INT_MAX) {
			if (step2)
        printf("-- %d\n", rank);
			return rank;
    }
    // Either query queue was empty or no valid words
    else {
			if (step2)
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
