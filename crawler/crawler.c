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
#include <string.h>
#include <queue.h>
#include <hash.h>
#include <webpage.h>

#define HASH_SIZE 100

bool compareURLs(void* page, const void* url);

int main() {

	// seed url
	char url[100];
	sprintf(url, "%s", "https://thayer.github.io/engs50/");

	// creates webpage using seed url
	webpage_t* web = webpage_new(url, 0, NULL);

    printf("%d\n", compareURLs(web, url));

	// fetch webpage html and check that it passes
	bool result_fetch = webpage_fetch(web);
	if (!result_fetch) {
		printf("webpage fecth failed\n");
		exit(EXIT_FAILURE);
	}


	// STEP 2: FETCH ONE PAGE
	// scan fetched html and print all URLs and whether each URL is
	// internal/external
	int position = 0;
	char* result;
	while ((position = webpage_getNextURL(web, position, &result)) > 0) {
		char* int_ext;
		if (IsInternalURL(result))
			int_ext = "internal";
		else
			int_ext = "external";
		printf("Found %s url: %s\n", int_ext, result);
		free(result);
	}
    
        
 
	// STEP 3: QUEUE OF WEBPAGES
	position = 0;
	queue_t* queue_urls = qopen();
	webpage_t* temp = NULL;

    hashtable_t* visited = hopen(HASH_SIZE);
    hput(visited, web, url, HASH_SIZE);
    
    char* testURL;
	// iterate through all URLs of the given page
	while ((position = webpage_getNextURL(web, position, &testURL)) > 0) {
		// if the URL is internal, create a new webpage for the URL and
		// place it in the queue

		if (IsInternalURL(testURL)) {
			temp = webpage_new(testURL, 1, NULL);
            
            // Check if page is in hash table, and if not, add it to the
            // queue and the hash table
            void* page = hsearch(visited, compareURLs, testURL, HASH_SIZE);
            
            if (page == NULL) {
			    qput(queue_urls, temp);
                hput(visited, temp, testURL, HASH_SIZE);
            }
		}
	}
    
	// prints the list of urls
	webpage_t* temp_page = (webpage_t*)qget(queue_urls);
	char* temp_url = NULL;
	while (temp_page != NULL) {
		temp_url = webpage_getURL(temp_page);
		printf("%s in queue\n", temp_url);
        webpage_delete(temp_page);
		temp_page = (webpage_t*)qget(queue_urls);	
	}

 	// close the queue
	qclose(queue_urls);
    hclose(visited);
	// deallocate webpages
 	webpage_delete(web);
	webpage_delete(temp_page);
	//webpage_delete(temp);
	
	exit(EXIT_SUCCESS);

}

bool compareURLs(void* page, const void* url) {
    char* castedURL = (char*) url;
    char* webpageURL = webpage_getURL((webpage_t*) page);
    

    bool result = strcmp(webpageURL, castedURL) == 0;
    printf("%d:\n%s\n%s\n\n", result, castedURL, webpageURL);
    return result;
}
