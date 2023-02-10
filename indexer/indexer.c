/* indexer.c --- 
 * 
 * 
 * Author: Raif T. Olson
 * Created: Fri Feb 10 12:55:31 2023 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <webpage.h>
#include <pageio.h>

int main(void) {
	webpage_t *webpage_1 = pageload(1, "../pages");
	int pos = 0;
	char *word;
	while ((pos = webpage_getNextWord(webpage_1, pos, &word)) > 0) {
		printf("%s\n", word);
		free(word);
	}
	return 0;
}
