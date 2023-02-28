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

void* putElement(void* argp) {
    lQueuePut_t* test = (lQueuePut_t*) argp;

    test->result = lqputcam(test->lqueue, test->elementp);
    
    return NULL;

}

void* getElement(void* argp) {
    lQueueGet_t* test = (lQueueGet_t*) argp;
    test->elementp = lqgetcam(test->lqueue);
    return NULL;

}
    
int main(void) {

	pthread_t tID1, tID2, tID3;
	
	lqueue_t* lqueue = lqopen();

	int* a = (int*)malloc(sizeof(int));
	*a = 10;
	int* b = (int*)malloc(sizeof(int));
	*b = 15;
												

	lQueuePut_t* lqpt = (lQueuePut_t*)malloc(sizeof(lQueuePut_t));
	lqpt -> lqueue = lqueue;
	lqpt -> elementp = a;

	lQueuePut_t* lqpt2 = (lQueuePut_t*)malloc(sizeof(lQueuePut_t));
	lqpt2 -> lqueue = lqueue;
	lqpt2 -> elementp = b;

	if (pthread_create(&tID1, NULL, putElement, lqpt) != 0) {
		printf("thread create failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&tID2, NULL, putElement, lqpt2) != 0) {
		printf("thread create failed 2\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(tID1, NULL) != 0) {
		printf("thread join failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_join(tID2, NULL) != 0) {
		printf("thread join failed\n");
		exit(EXIT_FAILURE);
	}
	printf("success: %d\n", lqpt->result);
    printf("success: %d\n", lqpt2->result);
    
    lQueueGet_t* lqgt = calloc(1, sizeof(lQueueGet_t));
    lqgt->lqueue = lqueue;
    lqgt->elementp = NULL;
    lQueueGet_t* lqgt2 = calloc(1, sizeof(lQueueGet_t));
    lqgt2->lqueue = lqueue;
    lqgt2->elementp = NULL;
    lQueueGet_t* lqgt3 = calloc(1, sizeof(lQueueGet_t));
    lqgt3->lqueue = lqueue;
    lqgt3->elementp = a;

    if (pthread_create(&tID1, NULL, getElement, lqgt) != 0) {
		printf("thread create failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&tID2, NULL, getElement, lqgt2) != 0) {
		printf("thread create failed 2\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(tID1, NULL) != 0) {
		printf("thread join failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_join(tID2, NULL) != 0) {
		printf("thread join failed\n");
		exit(EXIT_FAILURE);
	}
    printf("values: %d, %d\n", *(int*)lqgt->elementp, *(int*)lqgt2->elementp);

    if (pthread_create(&tID1, NULL, getElement, lqgt3) != 0) {
		printf("thread create failed\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_join(tID1, NULL) != 0) {
		printf("thread join failed\n");
		exit(EXIT_FAILURE);
	}
    if (lqgt3->elementp == NULL) {
        printf("Let's goooooooo\n");
    }

	//c = (int*)lqget(lqueue);
	//printf("result: %d\n", *c);
	
	lqapply(lqueue, freeElement);
	lqclose(lqueue);
	free(lqpt);
	free(lqpt2);
	free(lqgt);
	free(lqgt2);
    free(lqgt3);
    free(a);
    free(b);

	//free(b);
	
	exit(EXIT_SUCCESS);

}

