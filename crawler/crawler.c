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

	// scan fetched html and print all URLs and whether each URL is
	// internal/external
	int position = 0;
	char *result;

	while ((position = webpage_getNextURL(web, position, &result)) > 0) {
		char* int_ext;
		if (IsInternalURL(result))
			int_ext = "internal";
		else
			int_ext = "external";
		printf("Found %s url: %s\n", int_ext, result);
		free(result);
	}

	// deallocate webpage
 	webpage_delete(web);

	exit(EXIT_SUCCESS);

}
