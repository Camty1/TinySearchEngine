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
	// iterate through all URLs of the given page
	while ((position = webpage_getNextURL(web, position, &result)) > 0) {
		// if the URL is internal, create a new webpage for the URL and
		// place it in the queue
		if (IsInternalURL(result)) {
			temp = webpage_new(result, 1, NULL);
			qput(queue_urls, temp);
		}
		free(result);
	}
	// prints the list of urls
	webpage_t* temp_page = (webpage_t*)qget(queue_urls);
	char* temp_url = NULL;
	while (temp_page != NULL) {
		temp_url = webpage_getURL(temp_page);
		printf("%s in queue\n", temp_url);
		temp_page = (webpage_t*)qget(queue_urls);	
	}
 	// close the queue
	qclose(queue_urls);
	// deallocate webpages
 	webpage_delete(web);
	webpage_delete(temp_page);
	webpage_delete(temp);
	
	exit(EXIT_SUCCESS);

}
