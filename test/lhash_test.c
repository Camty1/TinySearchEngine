/* lhash_test.c ---
 *
 *
 * Author: Cameron J. Wolfe
 * Created: Fri Mar 3 15:10:49 2023 (-0500)
 * Version:
 *
 * Description:
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <lhash.h>
#include <time.h>

#define HASHTABLE_SIZE 100

typedef struct lhashResult {
    lhashtable_t* lhash;
    int result;
} lhashResult_t;

char* randomStr(int strlen);
void* putElement(void* arg);


int main() {
    
    lhashtable_t* lhash = lhopen(HASHTABLE_SIZE);

    if (lhash == NULL) {
        printf("lhopen() error\n");
        exit(EXIT_FAILURE);
    }

    lhashResult_t res = {lhash, 0};
    
    srand(time(NULL));
    int n = rand() % 6 + 4; // Get a random number between 4 and 10 

    pthread_t threads[n];

    int limits[] = {1, 2, 3, n};

    for (int i = 0; i < 4; i++) {

        for (int j = 0; j < limits[i]; j++) {
            
            if (pthread_create(&threads[j], NULL, putElement, &res) != 0) {
                printf("Thread (%d,%d) create failed\n", i, j);
                exit(EXIT_FAILURE);
            }
        }

        for (int k = 0; k < limits[i]; k++) {

            if (pthread_join(threads[k], NULL) != 0) {
                printf("Thread (%d, %d) join failed\n", i, k);
                exit(EXIT_FAILURE);
            }

            if (res.result != 0) {
                printf("Result failure (%d, %d)\n", i, k);
                exit(EXIT_FAILURE);
            }
        }

        printf("Passed putting elements with %d threads\n", limits[i]);
    }
    
    printf("Passed put tests\n");
    
    lhclose(lhash);
    exit(EXIT_SUCCESS);

}

void* putElement(void* arg) {
    
    lhashResult_t* res = (lhashResult_t*) arg;
    
    srand(time(NULL));
    int i = rand()%100;
    char* str = randomStr(i);

    res->result = lhput(res->lhash, &i, str, strlen(str));

    free(str);

    return NULL;

}

char* randomStr(int strlen) {
    char* str = malloc((strlen+1)*sizeof(char));
    
    for (int i = 0; i < strlen; i++) {
        str[i] = '0' + rand()%72;
    }

    str[strlen] = '\0';

    return str;
}
