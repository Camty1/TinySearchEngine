/* lqueue.c --- implementation of lqueue.h interface
 * 
 * 
 * Author: Hannah M. Brookes
 * Created: Tue Feb 28 19:01:26 2023 (-0500)
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
#include <unistd.h>


typedef struct internalLQueue {
	queue_t* queue;
	pthread_mutex_t* m;
} internalLQueue_t;


lqueue_t* lqopen(void) {
	// allocates memory for internal locked-queue type
	internalLQueue_t* lqueue = (internalLQueue_t*)malloc(sizeof(internalLQueue_t));
	if (lqueue == NULL)
		printf("error with allocation\n");
	// open queue in locked-queue structure 
	lqueue -> queue = qopen();
	// creates a mutex, initializes it, and adds to locked-queue
	// structure
	pthread_mutex_t* mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	lqueue -> m = mutex;
	return (lqueue_t*)lqueue;
}

void lqclose(lqueue_t *lqp) {
	// cast to internal queue type
	internalLQueue_t* lqueue = (internalLQueue_t*) lqp;
	// put lock on mutex
	pthread_mutex_lock(lqueue->m);
	// close the queue in locked-queue
	qclose(lqueue->queue);
	// unlock mutex
	pthread_mutex_unlock(lqueue->m);
	// destroy mutex
	pthread_mutex_destroy(lqueue->m);
	// free memory for mutext pointer and internal queue
	free(lqueue->m);
	free(lqueue);
}

int32_t lqput(lqueue_t *lqp, void *elementp) {
	// cast to internal queue type to access lock
	internalLQueue_t* lqueue = (internalLQueue_t*) lqp;
	// put lock on mutex
	pthread_mutex_lock(lqueue->m);
	// put the element in the queue
	int32_t result = qput(lqueue->queue, elementp);
	// unlock the mutex
	pthread_mutex_unlock(lqueue->m);
	return result;
}


void* lqget(lqueue_t* lqp) {
    internalLQueue_t* lqueue = (internalLQueue_t*) lqp;

    pthread_mutex_lock(lqueue->m);

    void* value = qget(lqueue->queue);

    pthread_mutex_unlock(lqueue->m);

    return value;

}

void lqapply(lqueue_t *lqp, void (*fn)(void *elementp)) {
	// cast to internal queue type to access lock
	internalLQueue_t* lqueue = (internalLQueue_t*) lqp;
	// put lock on mutex
	pthread_mutex_lock(lqueue->m);
	// use apply function from queue module
	qapply(lqueue->queue, fn);
	// unlock the mutex
	pthread_mutex_unlock(lqueue->m);
}
