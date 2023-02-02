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

	char url[100];
	sprintf(url, "%s", "https://thayer.github.io/engs50/");
  
	
	webpage_t* web = webpage_new(url, 0, NULL);

	bool result = webpage_fetch(web);
	if (!result) {
		printf("webpage fecth failed\n");
		exit(EXIT_FAILURE);
	}

	int weblen = webpage_getHTMLlen(web);


	printf("length html: %d\n", weblen);

}
