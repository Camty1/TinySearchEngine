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
#include <pthread.h>

void freeElement(void *elementp) {
	free(elementp);
}

int main(void) {

	pthread_t tID1, tID2;
	
	lqueue_t* lqueue = lqopen();

	int* a = (int*)malloc(sizeof(int));
	*a = 10;

	lQueuePut_t* lqpt = (lQueuePut_t*)malloc(sizeof(lQueuePut_t));
	lqpt -> lqueue = lqueue;
	lqpt -> elementp = a;

	if (pthread_create(&tID1, NULL, lqput, lqpt) != 0) {
		printf("thread create failed\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(tID1, NULL) != 0) {
		printf("thread join failed\n");
		exit(EXIT_FAILURE);
	}
	printf("success: %d\n", lqpt->result);
	
	/*
	int* b = (int*)lqget(lqueue);
	printf("result: %d\n", *b);
	*/
	lqapply(lqueue, freeElement);
	lqclose(lqueue);
	free(lqpt);

	//free(b);
	
	exit(EXIT_SUCCESS);

}

