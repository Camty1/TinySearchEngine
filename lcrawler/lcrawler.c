/* lcrawler.c --- 
 * 
 * 
 * Author: Hannah M. Brookes
 * Created: Thu Feb  2 11:06:04 2023 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <lqueue.h>
#include <lhash.h>
#include <webpage.h>
#include <pthread.h>
#include <pageio.h>

#define HASH_SIZE 100

typedef struct id_mutex {
    int value;
    pthread_mutex_t* m;
} id_mutex_t;

typedef struct flags_mutex {
    bool* flags;
    pthread_mutex_t* m;
} flags_mutex_t;

typedef struct crawler_args {
    lqueue_t* pageQueue;
    lhashtable_t* pageTable;
    lqueue_t* memQueue;
    id_mutex_t* id;
    flags_mutex_t* flags;
    int maxDepth;
    char* dirName;
    int numThreads;
} crawler_args_t;

typedef struct thread_args {
    crawler_args_t* args;
    int id;
} thread_args_t;

bool compareURLs(void* page, const void* url);
int32_t pagesave(webpage_t* pagep, int id, char* dirname);
void* crawl(void* cargs);
bool stillRunning(flags_mutex_t* mflags, int numThreads);


int main(int argv, char* argc[]) {
    
	// Check number of arguments
	if (argv != 5) {
		printf("usage: crawler <seedurl> <pagedir> <maxdepth> <numthreads>\n");
		exit(EXIT_FAILURE);
	}
	
	// Get arguments form argv
	char* url = argc[1];
	char* dirName = argc[2];
	char* endptr;
	uint32_t maxDepth = strtoul(argc[3], &endptr, 10);
  
	// Check if inputs are valid 
	if (endptr[0] == '-') {
		printf("maxdepth must be a positive integer\n");
		exit(EXIT_FAILURE);
	}
     
    uint32_t numThreads = strtoul(argc[4], &endptr, 10);
	
    // Check if inputs are valid 
	if (endptr[0] == '-') {
		printf("numthreads must be a positive integer\n");
		exit(EXIT_FAILURE);
	}
    
    
	// Create queue and hashtable
	queue_t* pageQueue = lqopen();

	hashtable_t* pageTable = lhopen(HASH_SIZE);

	// Create memory management queue
	queue_t* memQueue = lqopen();

    id_mutex_t* id = (id_mutex_t*) calloc(1, sizeof(id_mutex_t));
    if (id == NULL) {
        printf("Calloc Error\n");
        exit(EXIT_FAILURE);
    }
    id->value = 0;

    pthread_mutex_t* m = (pthread_mutex_t*) calloc(1, sizeof(pthread_mutex_t));
    if (m == NULL) {
        printf("Calloc Error\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(m, NULL);
    id->m = m;
    
    bool* flags = (bool*) calloc(numThreads, sizeof(bool));
    if (flags == NULL) {
        printf("Calloc Error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numThreads; i++) {
        flags[i] = false;
    }

    flags_mutex_t* mflags = (flags_mutex_t*) calloc(1, sizeof(flags_mutex_t));

    if (mflags == NULL) {
        printf("Calloc Error\n");
        exit(EXIT_FAILURE);
    }
    mflags->flags = flags;

    pthread_mutex_t* mf = (pthread_mutex_t*) calloc(1, sizeof(pthread_mutex_t));
    if (mf == NULL) {
        printf("Calloc Error\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(mf, NULL);

    mflags->m = mf;

    // Argument struct population
    crawler_args_t* args = (crawler_args_t*) calloc(1, sizeof(crawler_args_t));
    if (args == NULL) {
        printf("Calloc Error\n");
        exit(EXIT_FAILURE);
    }

    args->pageQueue = pageQueue;
    args->pageTable = pageTable;
    args->memQueue = memQueue;
    args->id = id;
    args->flags = mflags;
    args->maxDepth = maxDepth;
    args->dirName = dirName;
    args->numThreads = numThreads;

	// creates webpage using seed url
	webpage_t* web = webpage_new(url, 0, NULL);
	if (web == NULL) {
		printf("Error making webpage with url: %s\n", url);
		exit(EXIT_FAILURE);
	}
    
	// Put page into queue 
	int32_t qResult = lqput(pageQueue, web);
	if (qResult != 0) {
		printf("Error putting webpage into queue with url: %s\n", url);
		exit(EXIT_FAILURE);
	}
    
	// Put page into hashtable
	int32_t hResult = lhput(pageTable, web, url, strlen(url));
	if (hResult != 0) {
		printf("Error putting webpage into hashtable with url: %s\n", url);
		exit(EXIT_FAILURE);
	}
    
    // Create array of threads
    pthread_t* thread_list = calloc(numThreads, sizeof(pthread_t));

    for (int i = 0; i < numThreads; i++) {
        // Create and populate argument struct for current thread
        thread_args_t* thread_args = (thread_args_t*) calloc(1, sizeof(thread_args_t));
        thread_args->args = args;
        thread_args->id = i;

        // Run thread
        if (pthread_create(&thread_list[i], NULL, crawl, thread_args) != 0) {
            printf("Failed to create thread\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // Thread join
    for (int j = 0; j < numThreads; j++) {
        if (pthread_join(thread_list[j], NULL) != 0) {
            printf("Failed to join thread\n");
            exit(EXIT_FAILURE);
        }
    }

    free(thread_list);
    
    // Deal with webpage memory 
	webpage_t* memPage;
	while((memPage = lqget(memQueue)) != NULL) {
		webpage_delete(memPage);
	} 

 	// close the queue and hash
	lqclose(pageQueue);
	lqclose(memQueue);
	lhclose(pageTable);

	exit(EXIT_SUCCESS);
}

// Compare the url of a page and the given url
bool compareURLs(void* page, const void* url) {
	char* castedURL = (char*) url;
    webpage_t* castedPage = (webpage_t*) page;
	char* webpageURL = webpage_getURL(castedPage);
	bool result = (strcmp(webpageURL, castedURL) == 0);
	return result;
}

void* crawl(void* cargs) {
    // Struct unpacking
    thread_args_t* targs = (thread_args_t*) cargs;
    crawler_args_t* args = targs->args;
    lqueue_t* pageQueue = args->pageQueue;
    lhashtable_t* pageTable = args->pageTable;
    lqueue_t* memQueue = args->memQueue;
    id_mutex_t* id = args->id;
    flags_mutex_t* mflags = args->flags;
    int maxDepth = args->maxDepth;
    char* dirName = args->dirName;
    int numThreads = args->numThreads;

    int threadId = targs->id;

    bool* flags = mflags->flags;
    
    webpage_t* page;

    while (stillRunning(mflags, numThreads)) {

        if ((page = (webpage_t*) lqget(pageQueue)) != NULL) {
            
            pthread_mutex_lock(mflags->m);
            flags[threadId] = false;
            pthread_mutex_unlock(mflags->m);

            pthread_mutex_lock(id->m);
		    id->value++;
    
		    char* pageURL = webpage_getURL(page);

		    printf("%d: %s\n", id->value, pageURL);

		    // fetch webpage html and check that it passes
		    bool result_fetch = webpage_fetch(page);
		    if (!result_fetch) {
			    printf("webpage fecth failed with url: %s\n", webpage_getURL(page));
			    id->value--;
			    webpage_delete(page);
                pthread_mutex_unlock(id->m);
			    continue;
		    }
		    
            // Save page to directory and check that it passes
		    int32_t pagesave_result = pagesave(page, id->value, dirName);
		    if (pagesave_result == 1) {
			    printf("error saving page with url: %s\n", webpage_getURL(page));
			    exit(EXIT_FAILURE);
		    }

            pthread_mutex_unlock(id->m);

		    // Page crawling
		    int position = 0;
		    char* result;
		    int currentDepth = webpage_getDepth(page);
        
		    // Depth check 
		    if (currentDepth < maxDepth) {

			    // Go through all URLs on page
			    while ((position = webpage_getNextURL(page, position, &result)) >= 0) {

				    // Make sure url is internal
				    if (IsInternalURL(result)) {

					    // Create webpage object
					    webpage_t* crawled = webpage_new(result, currentDepth + 1, NULL);
					    char* pageURL = webpage_getURL(crawled);
    			    
					    // Check if page is in hash table, and if not, add it to the
					    // queue and the hash table
					    void* pageSearch = lhsearch(pageTable, compareURLs, pageURL, strlen(pageURL));
					    if (pageSearch == NULL) {
                         
						    // Since page hasn't been visited yet, put it in queue and table
						    int qresult = lqput(pageQueue, crawled);
						    int hresult = lhput(pageTable, crawled, pageURL, strlen(pageURL));
						    if (qresult != 0 || hresult != 0) {
							    printf("Error with queue and hashtable population\n");
							    exit(EXIT_FAILURE);
						    }
					    }
					    else {
						    webpage_delete(crawled);
                        }
					
				    }
						
				    // Deallocate result 
				    free(result);
			    }
		    } 
		    int32_t memResult = lqput(memQueue, page);
		    if (memResult != 0) {
			    printf("Error with memory queue\n");
			    exit(EXIT_FAILURE);
		    }
        }

        else {
            pthread_mutex_lock(mflags->m);
            flags[threadId] = true;
            pthread_mutex_unlock(mflags->m);
        }

    }

    return NULL;
}

bool stillRunning(flags_mutex_t* mflags, int numThreads) {
    
    bool* flags = mflags->flags;

    bool rval = false;
    
    pthread_mutex_lock(mflags->m);
    
    for (int i = 0; i < numThreads; i++) {
        if (flags[i] == false) {
            rval = true;
        }
    }

    pthread_mutex_unlock(mflags->m);

    return rval;
}
