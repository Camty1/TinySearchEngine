/* lhash.c ---
 *
 *
 * Author: Cameron J. Wolfe
 * Created: Fri Mar 3 13:49:14 2023 (-0500)
 * Version: 
 *
 * Description: 
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <hash.h>
#include <pthread.h>
#include <unistd.h>
#include <queue.h>
#include <lhash.h>

typedef struct lockedHash {
    hashtable_t* hash;
    pthread_mutex_t* m;
} lockedHash_t;

lhashtable_t* lhopen(uint32_t hsize) {
    
    // Internal Data Structure Allocation
    lockedHash_t* lhash = calloc(1, sizeof(lockedHash_t));
    if (lhash == NULL) {
        printf("Calloc error\n");
        return NULL;
    }

    // Creation of Hash table
    hashtable_t* hash = hopen(hsize);
    if (hash == NULL) {
        printf("hopen error\n");
        free(lhash);
        return NULL;
    }
    lhash->hash = hash;

    // Mutex Creation
    pthread_mutex_t* mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    if (mutex == NULL) {
        printf("mutex error\n");
        free(lhash);
        hclose(hash);
        return NULL;
    }
    lhash->m = mutex;
    
    // Return with type casting
    return (lhashtable_t*) lhash;
}

void lhclose(lhashtable_t* htp) {
    lockedHash_t* lhash = (lockedHash_t*) htp;
    
    pthread_mutex_lock(lhash->m);

    hclose(lhash->hash);

    pthread_mutex_unlock(lhash->m);

    pthread_mutex_destroy(lhash->m);

    free(lhash->m);
    free(lhash);
}

int32_t lhput(lhashtable_t* htp, void* ep, const char* key, int keylen) {
    lockedHash_t* lhash = (lockedHash_t*) htp;

    pthread_mutex_lock(lhash->m);

    int32_t status = hput(lhash->hash, ep, key, keylen);

    pthread_mutex_unlock(lhash->m);

    return status;

}

void lhapply(lhashtable_t *htp, void (*fn)(void* ep)) {

    lockedHash_t* lhash = (lockedHash_t*) htp;

    pthread_mutex_lock(lhash->m);

    happly(lhash->hash, fn);

    pthread_mutex_unlock(lhash->m);
}

void* lhsearch(lhashtable_t* htp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen) {

    lockedHash_t* lhash = (lockedHash_t*) htp;

    pthread_mutex_lock(lhash->m);

    void* result = hsearch(lhash->hash, searchfn, key, keylen);

    pthread_mutex_unlock(lhash->m);

    return result;

}

void* lhremove(lhashtable_t* htp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen) {

    lockedHash_t* lhash = (lockedHash_t*) htp;

    pthread_mutex_lock(lhash->m);

    void* result = hremove(lhash->hash, searchfn, key, keylen);

    pthread_mutex_unlock(lhash->m);

    return result;

}
