#pragma once
/* lhash.h ---
 *
 *
 * Author: Cameron J. Wolfe
 * Created: Fri Mar 3 13:37:32 2023 (-0500)
 * Version:
 *
 * Description: 
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <hash.h>

typedef void lhashtable_t;

lhashtable_t* lhopen(uint32_t hsize);

void lhclose(lhashtable_t* htp);

int32_t lhput(lhashtable_t *htp, 
        void* ep, 
        const char* key, 
        int keylen);

void lhapply(lhashtable_t *htp, void (*fn)(void* ep));

void* lhsearch(lhashtable_t *htp, 
        bool (*searchfn)(void* elementp, const void* searchkeyp), 
        const char *key, 
        int32_t keylen);

void* lhremove(lhashtable_t *htp, 
        bool (*searchfn)(void* elementp, const void* searchkeyp), 
        const char *key, 
        int32_t keylen);
