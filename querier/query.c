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
#include <indexer.h>

#define MAX_QUERY 200
#define MAX_DOCS 1000

static void printQueryWord(void* elementp);
static int normalizeWord(char *word, int word_len);
static int rankDocument(queue_t* queries, hashtable_t* index, int docId, bool step2);
static int getOccurence(hashtable_t* index, char* word, int docId);
static bool compareDocIDs(void* elementp, const void* keyp);
static bool compareId(void* elementp, const void* keyp);
static bool compareWord(void* elementp, const void* keyp);
static queue_t* documentIntersection(queue_t* query_q, hashtable_t* index);
static void printDocURL(int docID, char* pageDir, bool to_file);
static bool query_to_queue(queue_t* query_q, char* search_query);
static void printRanks(queue_t* query_q, hashtable_t* index, queue_t* intersection, char* pageDir, bool to_file);
static void printWordToFile(void* elementp);

FILE* outputFile;

int main(int argc, char* argv[]) {

    if (argc != 3 && argc != 6) {
        printf("usage: query <pageDir> <indexName> [-q]\nquery <pageDir> <indexName> -q <queryFile> <outputFile>\n");
        exit(EXIT_FAILURE);
    }
    else {
			  char* pageDir = argv[1];
        char* indexName = argv[2];

		   // Load in index		
				hashtable_t* index = index_all_pages(pageDir, indexName);
				if (index == NULL) {
					printf("empty or invalid directory\n");
					exit(EXIT_FAILURE);
				}
     
				else {
					if (argc == 3) {
						
						bool running = true;
						char search_query[MAX_QUERY];
						// Query prompt loop
						while (running) {
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
									printRanks(query_q, index, intersection, pageDir, false);
									qapply(intersection, removeDocCount);
									qclose(intersection);
									//				int rank = rankDocument(query_q, index, 1, true); // Step 2
								}					
								else {
									printf("[invalid query]\n");
								}
								qclose(query_q);
								// clears search query (beforehand, if hit return after a
								// valid search would use the first word of previous
								// query)
								search_query[0] = '\0';
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
					else {
						char* query_txt = argv[4];
						char* output_loc = argv[5];
						FILE* inputFile = fopen(query_txt, "r");
						outputFile = fopen(output_loc, "w");
						char search_query[MAX_QUERY];
						while (fgets(search_query, MAX_QUERY, inputFile)) {
							// if the line ends in a new-line character, set it to
							// \0 (otherwise will say all queries are invalid)
							int query_len = strlen(search_query);
							if (search_query[query_len-1] == '\n')
								search_query[query_len-1] = '\0';
							queue_t* query_q = qopen();
							bool query_valid = query_to_queue(query_q, search_query);
							if (query_valid) {
								qapply(query_q, printWordToFile);
								fprintf(outputFile, "\n");                                      
								queue_t* intersection;
								intersection = documentIntersection(query_q, index);
								printRanks(query_q, index, intersection, pageDir, true);
								qapply(intersection, removeDocCount);
								qclose(intersection);
								//				int rank = rankDocument(query_q, index, 1, true); // Step 2
							}					
							else {
								printf("[invalid query]\n");
							}
							qclose(query_q);
							
						}
						fclose(inputFile);	
					}
				}
        closeIndex(index);
    }

/*
    hashtable_t* index = indexLoad("../indexer/testFile");
	// do tests for valgrind.sh
	if (argc == 2) {
		char sample_query[MAX_QUERY];
		strcpy(sample_query, "good and coding ");
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
    */
	exit(EXIT_SUCCESS);
}

static void printWordToFile(void* elementp) {
	char* query = (char*) elementp;
	fprintf(outputFile, "%s ", query);                                                
}


static void printRanks(queue_t* query_q, hashtable_t* index, queue_t* intersection, char* pageDir, bool to_file) {
	queue_t* copy_intersection = qopen();
	int* first_doc_id = (int*)qget(intersection);
	qput(copy_intersection, first_doc_id);
	if (first_doc_id == NULL) {
		if (to_file)
			fprintf(outputFile, "%s", "no matching documents.\n");
		else
			printf("no matching documents.\n");
	}
	else {
		int* doc_id;
		int rank;
		rank = rankDocument(query_q, index, *first_doc_id, false);
		if (to_file) {
			fprintf(outputFile, "rank: %d, ", rank);
			fprintf(outputFile, "doc: %d, ", *first_doc_id);
			printDocURL(*first_doc_id, pageDir, true);
		}
		else {
			printf("rank: %d, ", rank);
			printf("doc: %d, ", *first_doc_id);
			printDocURL(*first_doc_id, pageDir, false);
		}
		while ((doc_id = (int*) qget(intersection)) != NULL) {
			qput(copy_intersection, doc_id);
			rank = rankDocument(query_q, index, *doc_id, false);
			if (to_file) {
				fprintf(outputFile, "rank: %d, ", rank);
				fprintf(outputFile, "doc: %d, ", *doc_id);
				printDocURL(*doc_id, pageDir, true);
			}
			else {
				printf("rank: %d, ", rank);
				printf("doc: %d, ", *doc_id);
				printDocURL(*doc_id, pageDir, false);
			}
		}
	}
	qconcat(intersection, copy_intersection);
}


static bool query_to_queue(queue_t* query_q, char* search_query) {
	// Go through words in query
	bool query_valid = true;
	bool first_word = true;
	bool curr_reserved = false;
	bool prev_reserved = false;
	char* query_word;
	char* prev_word;
	char delimit[]=" \t";
	query_word = strtok(search_query, delimit);
	while (query_word != NULL) {
		// Make sure word is valid
		if (normalizeWord(query_word, strlen(query_word)) == 0) {
			// check if current word is and | or;
			curr_reserved = strcmp(query_word, "and") == 0 || strcmp(query_word, "or") == 0;
			// if first word
			if (first_word) {
				// if word not and | or
				if (!curr_reserved) {
					// Put word in query queue
					qput(query_q, query_word);
				} else {
					query_valid = false;
					break;
				}
				first_word = false;
				// not first word
			} else {
				// check if previous word was and | or
				prev_reserved = strcmp(prev_word, "and") == 0 || strcmp(prev_word, "or") == 0;
				// if only one and | or in previous & current
				if (!prev_reserved || !curr_reserved) {
					// Put word in query queue
					qput(query_q, query_word);
					// both prev and curr words are and | or
				} else {
					query_valid = false;
					break;
				}
			}
			// normalize word failed
		} else {
			query_valid = false;
			break;
		}
		// store previous word
		prev_word = query_word;
		query_word = strtok(NULL, delimit);
	}
	if (curr_reserved)
		query_valid = false;
	return query_valid;
}

static void printDocURL(int docID, char* pageDir, bool to_file) {
	char buffer[100];
	snprintf(buffer, 100, "%s/%d", pageDir, docID);
	FILE* inputFile = fopen(buffer, "r");
	char url[1000];
	fgets(url, 1000, inputFile);
	fclose(inputFile);
	if (to_file)
		fprintf(outputFile, "%s", url);
	else
		printf("%s", url);
}

static queue_t* documentIntersection(queue_t* query_q, hashtable_t* index) {
	// creates copies of queues to deal with removing items without
	// losing information
	queue_t* copy_query_q = qopen();

	// different behavior if the word is the first in the set of "and" queries or not
	bool is_first_word = true;
	// queue to store the intersection of document IDs from all the "and" sets in
	// the query
	queue_t* intersection = qopen();
	// temporary intersection queue for each set of "and" queries
	queue_t* temp_intersection = qopen();
	char* word;
	docWordCount_t* doc;
	int* doc_id;
	// gets words in query queue one by one (each time removes word)
	while ((word = (char*) qget(query_q)) != NULL) {
		// if word is reserved put it back in the query_q without getting intersections
		if (strcmp(word, "or") == 0) {
			// reset to do first word in set of "and" queries
			is_first_word = true;
			// put reserved word in copy of queue to save for later
			qput(copy_query_q, word);
			
		} else if (strcmp(word, "and") == 0) {
			// do nothing
			// otherwise if word is longer than 2 letters
		} else if (strlen(word) > 2) {
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
					// add the previous set of intersections to the master list
					qconcat(intersection, temp_intersection);
					queue_t* copy_doc_q = qopen();
					temp_intersection = qopen();
					while((doc = (docWordCount_t*)qget(doc_q)) != NULL) {
						qput(copy_doc_q, doc);
						int* doc_id = (int*)malloc(sizeof(int*));
						*doc_id = doc->documentId;
						//						printf("doc_id: %d\n",*doc_id);
						// if docuement has already been added
						if (qsearch(intersection, compareDocIDs, doc_id)) {
								// get rid of it
							free(doc_id);
						} else {
							// otherwise add it to the temp list of intersection
							qput(temp_intersection, doc_id);
						}
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
					while ((doc_id = (int*)qget(temp_intersection)) != NULL) {
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
					qconcat(temp_intersection, copy_intersection);
				}
			}
			else {
				qclose(copy_query_q);
				//qapply(copy_doc_q, removeDocCount);
				//qclose(copy_doc_q);
				qapply(intersection, removeDocCount);
				qclose(intersection);
				return NULL;
			}
		}
	}
	// add the final set of intersections to the master list
	qconcat(intersection, temp_intersection);
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
	int occurences[MAX_QUERY];
	int i = 0;
	int occurence;
	int temp_rank = INT_MAX;
	int final_rank = 0;
	queue_t* copy_queries = qopen();
	// Go through words in query queue
	while ((word = (char*) qget(queries)) != NULL) {
		// put word back into temp queue for later
		qput(copy_queries, word);
		// If word is "or" then store rank for previous set of "and" queries
		if (strcmp(word, "or") == 0) {
			// store rank
			occurences[i] = temp_rank;
			i++;
			// reset rank for next set of "and" queries
			temp_rank = INT_MAX;
		} else if (strcmp(word, "and") == 0) {
			// do nothing
		}
		else {
			// Get number of occurences in index for a given document
			occurence = getOccurence(index, word, docId);
			// for testing purposes
			if (step2)
				printf("%s:%d ", word, occurence);

			// Tracking minimum occurence for this set of "and" queries
			if (occurence < temp_rank) {
				temp_rank = occurence;
			}
			
		}
	}
	// store the rank of the final set of "and" queries
	occurences[i] = temp_rank;
	// loop through the sets of "and" queries and add them together
	for(int j = 0; j<i+1; j++) {
		final_rank = final_rank + occurences[j];
	}
	
	// store the query back into the original queue
	qconcat(queries, copy_queries);
	// At least one word was valid and was given an occurence
	if (final_rank != INT_MAX && final_rank > 0) {
		if (step2)
			printf("-- %d\n", final_rank);
		return final_rank;
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

// Compare ints document ids
static bool compareDocIDs(void* elementp, const void* keyp) {
    int* id = (int*) elementp;
    int* key_id = (int*) keyp;
    return (*key_id == *id);
}
