/* crawler.c --- 
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
#include <queue.h>
#include <hash.h>
#include <webpage.h>

#define HASH_SIZE 100

bool compareURLs(void* page, const void* url);
int32_t pagesave(webpage_t* pagep, int id, char* dirname);
int32_t crawl(webpage_t* currentPage, int* index, int depth, queue_t* qp, hashtable_t* ht);

int main(int argv, char* argc[]) {
    
    // Check number of arguments
    if (argv != 4) {
        printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
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
     
    // Create queue and hashtable
    queue_t* pageQueue = qopen();
    hashtable_t* pageTable = hopen(HASH_SIZE);

	// creates webpage using seed url
	webpage_t* web = webpage_new(url, 0, NULL);
    if (web == NULL) {
        printf("Error making webpage with url: %s\n", url);
        exit(EXIT_FAILURE);
    }
    
    // Put page into queue 
    int32_t qResult = qput(pageQueue, web);
    if (qResult != 0) {
        printf("Error putting webpage into queue with url: %s\n", url);
        exit(EXIT_FAILURE);
    }
    
    // Put page into hashtable
    int32_t hResult = hput(pageTable, web, url, strlen(url));
    if (hResult != 0) {
        printf("Error putting webpage into hashtable with url: %s\n", url);
        exit(EXIT_FAILURE);
    }
    
	int id = 0;

    webpage_t* page;

    while ((page = (webpage_t*) qget(pageQueue)) != NULL) {

        id++;

	    // fetch webpage html and check that it passes
	    bool result_fetch = webpage_fetch(page);
	    if (!result_fetch) {
		    printf("webpage fecth failed with url: %s\n", webpage_getURL(page));
		    exit(EXIT_FAILURE);
	    }

        // Save page to directory and check that it passes
	    int32_t pagesave_result = pagesave(page, id, dirName);
	    if (pagesave_result == 0) {
		    printf("error saving page with url: %s\n", webpage_getURL(page));
		    exit(EXIT_FAILURE);
	    }
        
        // Page crawling
	    int position = 0;
	    char* result;
        int currentDepth = webpage_getDepth(page);
        if (currentDepth < maxDepth) {
	        while ((position = webpage_getNextURL(web, position, &result)) > 0) {
		        if (IsInternalURL(result)) {

    			    webpage_t* crawled = webpage_new(result, currentDepth + 1, NULL);
                
    			    // Check if page is in hash table, and if not, add it to the
    			    // queue and the hash table
    			    void* pageSearch = hsearch(pageTable, compareURLs, result, strlen(result)); 
        		    if (pageSearch == NULL) {
		    	    
                        // Since page hasn't been visited yet, put it in que and table
                        int qresult = qput(pageQueue, crawled);
			            int hresult = hput(pageTable, crawled, result, strlen(result));
                        if (qresult != 0 || hresult != 0) {
			                printf("Error with queue and hashtable population\n");
					        exit(EXIT_FAILURE);
				        }

			        }
                
                    // Page has been visited
			        else 
				        webpage_delete(crawled);
	        
                }
            // Deallocate result 
            free(result);
		    }
        } 
        webpage_delete(page);
    }
 	// close the queue and hash
	qclose(pageQueue);
	hclose(pageTable);

	exit(EXIT_SUCCESS);
}

// Compare the url of a page and the given url
bool compareURLs(void* page, const void* url) {
	char* castedURL = (char*) url;
	char* webpageURL = webpage_getURL((webpage_t*) page);
  
	bool result = strcmp(webpageURL, castedURL) == 0;
	return result;
}

int32_t pagesave(webpage_t* pagep, int id, char* dirname) {

	// retrieve relevant information about given webpage
	char *url = webpage_getURL(pagep);
	int depth = webpage_getDepth(pagep);
	int html_len = webpage_getHTMLlen(pagep);
	char *html = webpage_getHTML(pagep);
	
	// location of file
	char filepath[100];	
	sprintf(filepath, "%s/%d", dirname, id);	

	// open file (creates file if not exist, otherwise truncates file)
	FILE* output_file;
	output_file = fopen(filepath, "w+");
	
	if (output_file == NULL) {
		printf("error opening file\n");
		return 0;
	}

	// adds relevant information to file
	fprintf(output_file, "%s\n", url);
	fprintf(output_file, "%d\n", depth);
	fprintf(output_file, "%d\n", html_len);
	fprintf(output_file, "%s\n", html);

	// closes the file
	int success = fclose(output_file);
	if (success == 0)
		return 1;
	else
		return 0;
}
