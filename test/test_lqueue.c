/* test_lqueue.c --- 
 * 
 * 
 * Author: Hannah M. Brookes
 * Created: Wed Mar  1 10:39:10 2023 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdint.h>                                                                     
#include <stddef.h>                                                                     
#include <stdlib.h>                                                                     
#include <stdio.h>                                                                      
#include <queue.h>                                                                      
#include <lqueue.h>

void freeElement(void *elementp) {
	free(elementp);
}

int main(void) {

	printf("hello\n");

	lqueue_t* lqueue = lqopen();

	int* a = (int*)malloc(sizeof(int));
	*a = 10;

	int32_t result = lqput(lqueue, a);
	if (result != 0)
		printf("error with getting item\n");
	
	int* b = (int*)lqget(lqueue);
	printf("result: %d\n", *b);

	lqapply(lqueue, freeElement);
	lqclose(lqueue);

	free(b);
	
	printf("goodbye\n");
	exit(EXIT_SUCCESS);

}

