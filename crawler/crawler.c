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
    
	int id = 1;
	char *dirname = "../pages";

	//sprintf(dirname, "../pages");
	int32_t pagesave_result = pagesave(web, id, dirname);

	if (pagesave_result == 0) {
		printf("error saving page\n");
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
	qclose(queue_1);
 
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
	webpage_delete(temp_page);

	// STEP 5: SAVE ONE PAGE
	//int id = 1;
	//char *dirname = "../pages";

	//sprintf(dirname, "../pages");
	//int32_t pagesave_result = pagesave(web, id, dirname);

	//if (pagesave_result == 0) {
	//	printf("error saving page\n");
		//exit(EXIT_FAILURE);
	//}

					 
	
 	// close the queue and hash
	qclose(queue_urls);
	hclose(visited);
	// deallocate webpages
 	webpage_delete(web);
	

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
