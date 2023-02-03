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

	// fetch webpage html and check that it passes
	bool result_fetch = webpage_fetch(web);
	if (!result_fetch) {
		printf("webpage fecth failed\n");
		exit(EXIT_FAILURE);
	}


	// STEP 2: FETCH ONE PAGE
	// scan fetched html and print all URLs and whether each URL is
	// internal/external
	printf("STEP 2\n");
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
	queue_t* queue_1 = qopen();
	webpage_t* temp_q = NULL;
  position = webpage_getNextURL(web, position, &result);
	// iterate through all URLs of the given page
	while (position > 0) {
		// if the URL is internal, create a new webpage for the URL and
		// place it in the queue
		if (IsInternalURL(result)) {
			temp_q = webpage_new(result, 1, NULL);    
			qput(queue_1, temp_q);
		}
    free(result);
		position = webpage_getNextURL(web, position, &result);
	}
    
	// prints the list of urls
	printf("\nSTEP 3\n");
	webpage_t* temp_page_q = (webpage_t*)qget(queue_1);
	char* temp_url_q = NULL;
	while (temp_page_q != NULL) {
		temp_url_q = webpage_getURL(temp_page_q);
		printf("%s in queue\n", temp_url_q);
		webpage_delete(temp_page_q);
		temp_page_q = (webpage_t*)qget(queue_1);	
	}
	webpage_delete(temp_page_q);

 
	// STEP 4: HASHTABLE OF URLs
	position = 0;
	queue_t* queue_urls = qopen();
	webpage_t* temp = NULL;

	hashtable_t* visited = hopen(HASH_SIZE);
	int hresult = hput(visited, web, url, strlen(url));
	
	if (hresult != 0) {
		printf("Failed putting original webpage in hashtable\n");
		exit(EXIT_FAILURE);
	}
    
	position = webpage_getNextURL(web, position, &result);

	//    printf("position: %d\n", position);
	// iterate through all URLs of the given page
	while (position > 0) {
		// if the URL is internal, create a new webpage for the URL and
		// place it in the queue

		if (IsInternalURL(result)) {
			temp = webpage_new(result, 1, NULL);
            
			// Check if page is in hash table, and if not, add it to the
			// queue and the hash table
			void* page = hsearch(visited, compareURLs, result, strlen(result)); 
      
			if (page == NULL) {
			    int qresult = qput(queue_urls, temp);
					int hresult = hput(visited, temp, result, strlen(result));
					if (qresult != 0 || hresult != 0) {
						printf("Error with queue and hashtable population\n");
						exit(EXIT_FAILURE);
					}
			}
			else {
				webpage_delete(temp);
			}
		}
		free(result);
		position = webpage_getNextURL(web, position, &result);
				//        printf("position: %d\n", position);

	}
    
	// prints the list of urls
	printf("\nSTEP 4\n");
	webpage_t* temp_page = (webpage_t*)qget(queue_urls);
	char* temp_url = NULL;
	while (temp_page != NULL) {
		temp_url = webpage_getURL(temp_page);
		printf("%s in queue\n", temp_url);
		webpage_delete(temp_page);
		temp_page = (webpage_t*)qget(queue_urls);	
	}

 	// close the queues
	qclose(queue_1);
	qclose(queue_urls);
	hclose(visited);
	// deallocate webpages
 	webpage_delete(web);
	webpage_delete(temp_page);

	exit(EXIT_SUCCESS);

}

// Compare the url of a page and the given url
bool compareURLs(void* page, const void* url) {
	char* castedURL = (char*) url;
	char* webpageURL = webpage_getURL((webpage_t*) page);
  
	bool result = strcmp(webpageURL, castedURL) == 0;
	return result;
}
