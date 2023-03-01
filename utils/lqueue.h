#pragma once
/* lqueue.h --- 
 * 
 * 
 * Author: Hannah M. Brookes
 * Created: Tue Feb 28 19:03:13 2023 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */
#include <stdint.h>
#include <stdbool.h>
#include <queue.h>

typedef void lqueue_t;

typedef struct lQueuePut {
	lqueue_t* lqueue;
	void* elementp;
	int32_t result;
} lQueuePut_t;

typedef struct lQueueGet {
	lqueue_t* lqueue;
	void* elementp;
} lQueueGet_t;

lqueue_t* lqopen(void);

void lqclose(lqueue_t *lqp);

int32_t lqput(lqueue_t *lqp, void *elementp);

void* lqget(lqueue_t *lqp);

void lqapply(lqueue_t *lqp, void (*fn)(void* elementp));

void lqsearch(lqueue_t *lqp,
							bool (*searchfn)(void *elementp, const void *keyp),
							const void *skeyp);
