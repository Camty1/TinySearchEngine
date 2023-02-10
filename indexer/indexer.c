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
#include <string.h>
#include <ctype.h>
#include <webpage.h>
#include <pageio.h>


static int NormalizeWord(char *word, int word_len) {
	if (word_len > 2) {
		for (int i=0; i<word_len; i++) {
			char letter = word[i];
			int res = isalpha(letter);
			if (res != 0) {
				if ((64 < letter) && (letter < 91)) {
					word[i] = tolower(letter);
				}
			} else {
				return -1;
			}
		}
	}
	else {
		return -1;
	}
	return 0;
}

int main(void) {
	webpage_t *webpage_1 = pageload(1, "../pages");
	int pos = 0;
	int prev_pos = 0;
	char *word;
	while ((pos = webpage_getNextWord(webpage_1, pos, &word)) > 0) {
		int res = NormalizeWord(word, (pos-prev_pos));
		if (res == 0) printf("%s\n", word);
		free(word);
		prev_pos = pos;
	}
	return 0;
}
